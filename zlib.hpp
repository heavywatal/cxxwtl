#pragma once
#ifndef WTL_ZLIB_HPP_
#define WTL_ZLIB_HPP_

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <zlib.h>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace zlib {
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

class Zstream : public z_stream {
  public:
    Zstream() {
        this->zalloc = Z_NULL;
        this->zfree = Z_NULL;
        this->opaque = Z_NULL;
        auto ret = deflateInit2(this, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK) throw Exception(*this, ret);
    }
    ~Zstream() {
        deflateEnd(this);
    }
};

class ostreambuf : public std::streambuf {
    static constexpr std::streamsize SIZE = 4096;
  public:
    ostreambuf(std::streambuf* writer)
    : writer_(writer) {
        assert(writer_);
        setp(in_buf_, in_buf_ + SIZE);
    }
    ostreambuf(const ostreambuf&) = delete;
    ostreambuf(ostreambuf&&) = default;
    ostreambuf& operator=(const ostreambuf&) = delete;
    ostreambuf& operator=(ostreambuf&&) = default;
    ~ostreambuf() {sync();}

    int sync() override {
        overflow();
        zstrm_.next_in = nullptr;
        assert(zstrm_.avail_in == 0);
        while (deflate_write(Z_FINISH) != Z_STREAM_END) {;}
        deflateReset(&zstrm_);
        return 0;
    }

    std::streambuf::int_type overflow(std::streambuf::int_type c = traits_type::eof()) override {
        zstrm_.avail_in = pptr() - pbase();
        zstrm_.next_in = reinterpret_cast<decltype(zstrm_.next_in)>(pbase());
        while (zstrm_.avail_in > 0) {
            deflate_write(Z_NO_FLUSH);
        }
        setp(in_buf_, in_buf_ + SIZE);
        return traits_type::eq_int_type(c, traits_type::eof()) ? traits_type::eof() : sputc(c);
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

  private:
    char in_buf_[SIZE];
    char out_buf_[SIZE];
    Zstream zstrm_;
    std::streambuf* writer_;
};

class Initializer {
  public:
    Initializer(const std::string& filename, std::ios_base::openmode mode)
    : ofs_(filename, mode), ostbuf_(ofs_.rdbuf()) {
        ofs_.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    }
  private:
    std::ofstream ofs_;
  protected:
    ostreambuf ostbuf_;
};

class ofstream : private Initializer, public std::ostream {
  public:
    explicit ofstream(const std::string& filename,
                      std::ios_base::openmode mode = std::ios_base::out)
    : Initializer(filename, mode | std::ios_base::binary),
      std::ostream(&ostbuf_) {
        exceptions(std::ios_base::failbit | std::ios_base::badbit);
    }
    const std::string& path() const noexcept {return path_;}
  private:
    const std::string path_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::zlib
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ZLIB_HPP_
