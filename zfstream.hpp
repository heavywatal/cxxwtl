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

class ozfstream: public boost::iostreams::filtering_ostream {
  public:
    ozfstream(const std::string& path, std::ios::openmode mode=std::ios::out):
      boost::iostreams::filtering_ostream() {
        if (std::regex_search(path, std::regex("\\.gz$"))) {
            push(boost::iostreams::gzip_compressor());
        }
        push(boost::iostreams::file_descriptor_sink(path, mode));
    }
};

class izfstream: public boost::iostreams::filtering_istream {
  public:
    izfstream(const std::string& path, std::ios::openmode mode=std::ios::in):
      boost::iostreams::filtering_istream(),
      ifs_(path, mode) {
        if (std::regex_search(path, std::regex("\\.gz$"))) {
            push(boost::iostreams::gzip_decompressor());
        }
        push(ifs_);
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

    // WORKAROUND: inherited one does not work
    bool operator !() const {return !ifs_;}
  private:
    std::ifstream ifs_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ZFSTREAM_HPP_
