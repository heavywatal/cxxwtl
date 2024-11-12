#pragma once
#ifndef WTL_ZFSTREAM_HPP_
#define WTL_ZFSTREAM_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace wtl {

namespace fs = std::filesystem;
namespace bios = boost::iostreams;

class [[deprecated("use wtl::zlib::ofstream")]] ozfstream: public bios::filtering_ostream {
  public:
    ozfstream(const fs::path& path, std::ios::openmode mode=std::ios::out):
      bios::filtering_ostream(), path_(path) {
        if (path.extension() == ".gz") {
            push(bios::gzip_compressor());
        }
        push(bios::file_descriptor_sink(path, mode));
    }
    const fs::path& path() const {return path_;}
  private:
    const fs::path path_;
};

class [[deprecated("use wtl::zlib::ifstream")]] izfstream: public bios::filtering_istream {
  public:
    izfstream(const fs::path& path, std::ios::openmode mode=std::ios::in):
      bios::filtering_istream(), path_(path) {
        if (path.extension() == ".gz") {
            push(bios::gzip_decompressor());
        }
        push(bios::file_descriptor_source(path, mode));
    }
    std::string readline(const char delimiter='\n') {
        std::string buffer;
        std::getline(*this, buffer, delimiter);
        return buffer;
    }
    std::vector<std::string> readlines(const char delimiter='\n') {
        std::vector<std::string> lines;
        std::string buffer;
        while (std::getline(*this, buffer, delimiter)) {
            lines.push_back(buffer);
        }
        return lines;
    }
    std::string read(const char delimiter='\0') {return readline(delimiter);}
    void close() {pop();}
    const fs::path& path() const {return path_;}
  private:
    const fs::path path_;
};

} // namespace wtl

#endif // WTL_ZFSTREAM_HPP_
