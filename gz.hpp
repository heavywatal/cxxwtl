// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_GZ_HPP_
#define WTL_GZ_HPP_

#include <fstream>
#include <string>
#include <vector>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class ogzstream: public boost::iostreams::filtering_ostream {
  public:
    ogzstream(const std::string& path):
        boost::iostreams::filtering_ostream(boost::iostreams::gzip_compressor()) {
        this->push(boost::iostreams::file_descriptor_sink(path));
    }
};

class igzstream: public boost::iostreams::filtering_istream {
  public:
    igzstream(const std::string& path):
        boost::iostreams::filtering_istream(boost::iostreams::gzip_decompressor()), ifs_(path) {
        this->push(ifs_);
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

    // WORKAROUND: inherited one does not work
    bool operator !() const {return !ifs_;}
  private:
    std::ifstream ifs_;
};

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

#endif // WTL_GZ_HPP_
