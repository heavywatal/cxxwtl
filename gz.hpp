// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef GZ_HPP_
#define GZ_HPP_

#include <string>
#include <ostream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class gzip {
  public:
    gzip(): filter_(boost::iostreams::gzip_compressor()) {;}
    gzip(std::ostream& ofs): gzip() {
        filter_.push(ofs);
    }
    gzip(std::ostream&& ofs): gzip() {
        filter_.push(ofs);
    }
    template <class T>
    gzip& operator<< (const T& x) {
        filter_ << x;
        return *this;
    }

  private:
    boost::iostreams::filtering_ostream filter_;
    gzip(const gzip &) = delete;
    gzip &operator=(const gzip &) = delete;
};

class gunzip {
  public:
    gunzip(): filter_(boost::iostreams::gzip_decompressor()) {;}
    gunzip(std::istream& ifs): gunzip{} {
        filter_.push(ifs);
    }
    gunzip(std::istream&& ifs): gunzip{} {
        filter_.push(ifs);
    }
    template <class T>
    gunzip& operator>> (const T& x) {
        x >> filter_;
        return *this;
    }
    std::string readline(const char delimiter='\n') {
        std::string buffer;
        std::getline(filter_, buffer, delimiter);
        return buffer;
    }
    
    std::vector<std::string> readlines(const char delimiter='\n') {
        std::vector<std::string> lines;
        std::string buffer;
        while (std::getline(filter_, buffer, delimiter)) {
            lines.push_back(buffer);
        }
        return lines;
    }
    
    std::string read(const char delimiter='\0') {return readline(delimiter);}

  private:
    boost::iostreams::filtering_istream filter_;
    gunzip(const gunzip &) = delete;
    gunzip &operator=(const gunzip &) = delete;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // GZ_HPP_
