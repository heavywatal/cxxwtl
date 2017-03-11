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
    ogzstream(const std::string& path, std::ios::openmode mode=std::ios::out):
        boost::iostreams::filtering_ostream(boost::iostreams::gzip_compressor()) {
        this->push(boost::iostreams::file_descriptor_sink(path, mode));
    }
};

class igzstream: public boost::iostreams::filtering_istream {
  public:
    igzstream(const std::string& path, std::ios::openmode mode=std::ios::in):
        boost::iostreams::filtering_istream(boost::iostreams::gzip_decompressor()), ifs_(path, mode) {
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
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_GZ_HPP_
