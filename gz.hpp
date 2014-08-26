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
    gzip(std::ostream& ofs) {
        filter_.push(ofs);
    }
    gzip(std::ostream&& ofs) {
        filter_.push(ofs);
    }

    template <class T>
    gzip& operator<< (const T& x) {
        filter_ << x;
        return *this;
    }

    operator std::ostream&() {return filter_;}

  private:
    boost::iostreams::filtering_ostream filter_{boost::iostreams::gzip_compressor()};
    gzip(const gzip &) = delete;
    gzip &operator=(const gzip &) = delete;
};

class gunzip {
  public:
    gunzip(std::istream& ifs) {
        filter_.push(ifs);
    }
    gunzip(std::istream&& ifs) {
        filter_.push(ifs);
    }

    template <class T>
    gunzip& operator>> (T& x) {
        filter_ >> x;
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

    operator std::istream&() {return filter_;}

  private:
    boost::iostreams::filtering_istream filter_ {boost::iostreams::gzip_decompressor()};
    gunzip(const gunzip &) = delete;
    gunzip &operator=(const gunzip &) = delete;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // GZ_HPP_
