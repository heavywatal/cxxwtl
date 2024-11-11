#pragma once
#ifndef WTL_ZLIB_HPP_
#define WTL_ZLIB_HPP_

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>
#include <zlib.h>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace zlib {

namespace fs = std::filesystem;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class Exception : public std::runtime_error {
  public:
    Exception(const z_stream& zstrm, int ret)
    : std::runtime_error("") {
        std::ostringstream oss;
        oss << "wtl::zlib::";
        switch (ret) {
          case Z_STREAM_ERROR:  oss << "Z_STREAM_ERROR: ";  break;
          case Z_DATA_ERROR:    oss << "Z_DATA_ERROR: ";    break;
          case Z_MEM_ERROR:     oss << "Z_MEM_ERROR: ";     break;
          case Z_BUF_ERROR:     oss << "Z_BUF_ERROR: ";     break;
          case Z_VERSION_ERROR: oss << "Z_VERSION_ERROR: "; break;
          default:              oss << "[" << ret << "]: "; break;
        }
        oss << zstrm.msg;
        what_ = oss.str();
    }
    const char* what() const noexcept override {
        return what_.c_str();
    }
  private:
    std::string what_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace detail {

class iz_stream : public z_stream {
  public:
    iz_stream() {
        this->zalloc = Z_NULL;
        this->zfree = Z_NULL;
        this->opaque = Z_NULL;
        this->next_in = Z_NULL;
        this->avail_in = 0;
        int ret = inflateInit2(this, MAX_WBITS + 32);
        if (ret != Z_OK) throw Exception(*this, ret);
    }
    ~iz_stream() {inflateEnd(this);}
};

class oz_stream : public z_stream {
  public:
    oz_stream() {
        this->zalloc = Z_NULL;
        this->zfree = Z_NULL;
        this->opaque = Z_NULL;
        int ret = deflateInit2(this, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK) throw Exception(*this, ret);
    }
    ~oz_stream() {deflateEnd(this);}
};

} // namespace detail
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class istreambuf : public std::streambuf {
    static constexpr std::streamsize SIZE = 4096;
  public:
    istreambuf(std::streambuf* reader)
    : reader_(reader) {
        setg(out_buf_, out_buf_, out_buf_);
    }
    istreambuf(const istreambuf&) = delete;
    istreambuf(istreambuf&&) = default;
    istreambuf& operator=(const istreambuf&) = delete;
    istreambuf& operator=(istreambuf&&) = default;

    int_type underflow() final {
        if (zstrm_.avail_in == 0) {
            std::streamsize num_copied = reader_->sgetn(in_buf_, SIZE);
            zstrm_.next_in = reinterpret_cast<decltype(zstrm_.next_in)>(in_buf_);
            zstrm_.avail_in = static_cast<decltype(zstrm_.avail_in)>(num_copied);
        }
        zstrm_.next_out = reinterpret_cast<decltype(zstrm_.next_out)>(out_buf_);
        zstrm_.avail_out = SIZE;
        int ret = inflate(&zstrm_, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END) throw Exception(zstrm_, ret);
        this->setg(out_buf_, out_buf_, reinterpret_cast<char*>(zstrm_.next_out));
        return this->gptr() == this->egptr()
            ? traits_type::eof()
            : traits_type::to_int_type(*this->gptr());
    }
  private:
    char in_buf_[SIZE];
    char out_buf_[SIZE];
    detail::iz_stream zstrm_;
    std::streambuf* reader_;
};

class ostreambuf : public std::streambuf {
    static constexpr std::streamsize SIZE = 4096;
  public:
    ostreambuf(std::streambuf* writer)
    : writer_(writer) {
        setp(in_buf_, in_buf_ + SIZE);
    }
    ostreambuf(const ostreambuf&) = delete;
    ostreambuf(ostreambuf&&) = default;
    ostreambuf& operator=(const ostreambuf&) = delete;
    ostreambuf& operator=(ostreambuf&&) = default;
    ~ostreambuf() {
        sync();
        deflateReset(&zstrm_);
    }

    int_type overflow(int_type c = traits_type::eof()) final {
        zstrm_.next_in = reinterpret_cast<decltype(zstrm_.next_in)>(pbase());
        zstrm_.avail_in = static_cast<decltype(zstrm_.avail_in)>(pptr() - pbase());
        while (zstrm_.avail_in > 0) {
            deflate_write(Z_NO_FLUSH);
        }
        setp(in_buf_, in_buf_ + SIZE);
        return traits_type::eq_int_type(c, traits_type::eof())
            ? traits_type::eof()
            : sputc(static_cast<char>(c));
    }

    int deflate_write(int flush) {
        zstrm_.next_out = reinterpret_cast<decltype(zstrm_.next_out)>(out_buf_);
        zstrm_.avail_out = SIZE;
        int ret = deflate(&zstrm_, flush);
        if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
            throw Exception(zstrm_, ret);
        }
        std::streamsize writing = SIZE - zstrm_.avail_out;
        std::streamsize written = writer_->sputn(out_buf_, writing);
        if (written != writing) {
            throw std::ios_base::failure("wtl::zlib: written != writing");
        }
        return ret;
    }

  protected:
    int sync() final {
        overflow();
        zstrm_.next_in = nullptr;
        while (deflate_write(Z_FINISH) != Z_STREAM_END) {;}
        return std::streambuf::sync();
    }
  private:
    char in_buf_[SIZE];
    char out_buf_[SIZE];
    detail::oz_stream zstrm_;
    std::streambuf* writer_;
};

namespace detail {

template <class Stream>
class FstrInitializer {
    static constexpr bool is_ist = std::is_same_v<std::istream, Stream>;
    using Fstream = typename std::conditional<is_ist, std::ifstream, std::ofstream>::type;
    using StreamBuf = typename std::conditional<is_ist, istreambuf, ostreambuf>::type;
  public:
    FstrInitializer(const fs::path& path, std::ios_base::openmode mode)
    : fst_(path, mode | std::ios_base::binary),
      strbuf_(fst_.rdbuf()) {}
  protected:
    Fstream fst_;
    StreamBuf strbuf_;
};

template <class Stream>
class SstrInitializer {
    static constexpr bool is_ist = std::is_same_v<std::istream, Stream>;
    using StringStream = typename std::conditional<is_ist, std::istringstream, std::ostringstream>::type;
    using StreamBuf = typename std::conditional<is_ist, istreambuf, ostreambuf>::type;
  public:
    SstrInitializer()
    : sst_(std::ios_base::binary),
      strbuf_(sst_.rdbuf()) {}
  protected:
    StringStream sst_;
    StreamBuf strbuf_;
};

} // namespace detail

template<class Stream>
class basic_fstream
: private detail::FstrInitializer<Stream>,
  public Stream {
  public:
    explicit basic_fstream(const fs::path& p,
                           std::ios_base::openmode mode=std::ios_base::binary)
    : detail::FstrInitializer<Stream>(p, mode),
      Stream(&this->strbuf_),
      path_(p) {
        this->fst_.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        this->exceptions(std::ios_base::badbit);
    }
    const fs::path& path() const noexcept {return path_;}
  private:
    const fs::path path_;
};

using ifstream = basic_fstream<std::istream>;
using ofstream = basic_fstream<std::ostream>;

template<class Stream>
class basic_stringstream
: private detail::SstrInitializer<Stream>,
  public Stream {
  public:
    explicit basic_stringstream(std::string_view str = "")
    : detail::SstrInitializer<Stream>(),
      Stream(&this->strbuf_) {
        this->sst_.exceptions(std::ios_base::badbit);
        this->exceptions(std::ios_base::badbit);
        *this << str;
      }
    std::string str() {
      this->strbuf_.pubsync();
      return this->sst_.str();
    }
};

using istringstream = basic_stringstream<std::istream>;
using ostringstream = basic_stringstream<std::ostream>;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::zlib
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ZLIB_HPP_
