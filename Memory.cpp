// Memory.cpp
//
// Copyright 2013 Jesse Haber-Kucharsky
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Memory.hpp"

DEFINE_LOGGER( MEMORY, "Memory" )

#include <algorithm>
#include <iterator>
#include <fstream>
#include <thread>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>

namespace nebula {

Word Memory::read( Word offset ) {
    if ( offset >= size() ) {
        throw error::InvalidMemoryLocation {
            MemoryOperation::Read,
            offset
        };    
    }
    
    std::lock_guard<std::mutex> lock { _mutex };

    std::this_thread::sleep_for( MEMORY_READ_DURATION );
    return _vec[offset];
}

void Memory::write( Word offset, Word value ) {
    if ( offset >= size() ) {
        throw error::InvalidMemoryLocation {
            MemoryOperation::Write,
            offset
        };
    }

    std::lock_guard<std::mutex> lock { _mutex };
    std::this_thread::sleep_for( MEMORY_WRITE_DURATION );
    _vec[offset] = value;
}

int Memory::size() {
    std::lock_guard<std::mutex> lock { _mutex };
    return _vec.size();
}

void Memory::dumpToFile( const std::string& filename,
                         std::shared_ptr<Memory> mem,
                         ByteOrder order ) {
    namespace karma = boost::spirit::karma;

    std::ofstream file {
        filename,
        std::ios::out | std::ios::binary
    };

    if ( ! file.is_open() ) {
        throw error::UnwritableMemoryFile { filename };
    }

    for ( const auto& w : mem->_vec ) {
        if ( order == ByteOrder::BigEndian ) {
            file << karma::format( karma::big_word, w );
        } else {
            file << karma::format( karma::little_word, w );
        }
    }

    LOG( MEMORY, info ) << format( "Wrote memory dump to '%s'" ) % filename;

    file.close();
}

std::shared_ptr<Memory>
Memory::fromFile( const std::string& filename, int size, ByteOrder order ) {
    namespace qi = boost::spirit::qi;

    std::ifstream file {
        filename,
        std::ios::in | std::ios::binary
    };

    if ( ! file.is_open() ) {
        throw error::MissingMemoryFile { filename };
    }

    auto begin = std::istreambuf_iterator<char> { file };
    auto end = std::istreambuf_iterator<char> {};

    std::vector<char> contents( begin, end );
    std::vector<Word> result;

    bool isSuccessful { false };

    switch ( order ) {
    case ByteOrder::BigEndian:
        isSuccessful = qi::parse( contents.begin(), contents.end(),
                                  +qi::big_word,
                                  result );
        break;
    case ByteOrder::LittleEndian:
        isSuccessful = qi::parse( contents.begin(), contents.end(),
                                  +qi::little_word,
                                  result );
        break;
    }

    if ( ! isSuccessful ) {
        throw error::BadMemoryFile {};
    }

    if ( result.size() > static_cast<unsigned>( size )) {
        throw error::MemoryFileTooBig { size };
    }

    auto mem = std::make_shared<Memory>( size );
    std::move( result.begin(), result.end(), mem->_vec.begin() );

    LOG( MEMORY, info ) << format( "Read memory successfully from '%s'" ) % filename;

    return mem;
}

}
