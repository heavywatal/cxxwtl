// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_ZFSTREAM_HPP_
#define WTL_ZFSTREAM_HPP_

#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace bios = boost::iostreams;

class ozfstream: public bios::filtering_ostream {
  public:
    ozfstream(const std::string& path, std::ios::openmode mode=std::ios::out):
      bios::filtering_ostream(), path_(path) {
        if (std::regex_search(path, std::regex("\\.gz$"))) {
            push(bios::gzip_compressor());
        }
        push(bios::file_descriptor_sink(path, mode));
    }
    const std::string& path() const {return path_;}
  private:
    const std::string path_;
};

class izfstream: public bios::filtering_istream {
  public:
    izfstream(const std::string& path, std::ios::openmode mode=std::ios::in):
      bios::filtering_istream(), path_(path) {
        if (std::regex_search(path, std::regex("\\.gz$"))) {
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
    const std::string& path() const {return path_;}
  private:
    const std::string path_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ZFSTREAM_HPP_
