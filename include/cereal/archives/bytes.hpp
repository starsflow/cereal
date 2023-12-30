/*! \file binary.hpp
    \brief Bytes input and output archives */
/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_ARCHIVES_BYTES_HPP_
#define CEREAL_ARCHIVES_BYTES_HPP_

#include "cereal/cereal.hpp"
#include <cstddef>
#include <cstring>
#include <sstream>

namespace cereal
{
  // ######################################################################
  //! An output archive designed to save data in a compact binary representation
  /*! This archive outputs data to a stream in an extremely compact binary
      representation with as little extra metadata as possible.

      This archive does nothing to ensure that the endianness of the saved
      and loaded data is the same.  If you need to have portability over
      architectures with different endianness, use PortableBinaryOutputArchive.

      When using a binary archive and a file stream, you must use the
      std::ios::binary format flag to avoid having your data altered
      inadvertently.

      \ingroup Archives */
  class BytesOutputArchive : public OutputArchive<BytesOutputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      BytesOutputArchive(char* bytes) :
        OutputArchive<BytesOutputArchive, AllowEmptyClassElision>(this),
        itsBytes(bytes)
      { }

      ~BytesOutputArchive() CEREAL_NOEXCEPT = default;

      //! Writes size bytes of data to the output stream
      void saveBytes( const void * data, std::streamsize size )
      {
        memcpy(itsBytes, data, size);
        itsBytes += size;
        byteSize += size;
      }

    std::streamsize getSize() {
        return byteSize;
    }

    private:
      char* itsBytes;
      std::streamsize byteSize = 0;
  };

  // ######################################################################
  //! An input archive designed to load data saved using BinaryOutputArchive
  /*  This archive does nothing to ensure that the endianness of the saved
      and loaded data is the same.  If you need to have portability over
      architectures with different endianness, use PortableBinaryOutputArchive.

      When using a binary archive and a file stream, you must use the
      std::ios::binary format flag to avoid having your data altered
      inadvertently.

      \ingroup Archives */
  class BytesInputArchive : public InputArchive<BytesInputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, loading from the provided stream
      BytesInputArchive(const char* bytes) :
        InputArchive<BytesInputArchive, AllowEmptyClassElision>(this),
        itsBytes(const_cast<char*>(bytes))
      { }

      ~BytesInputArchive() CEREAL_NOEXCEPT = default;

      //! Reads size bytes of data from the input stream
      void loadBytes( void * const data, std::streamsize size )
      {
        memcpy(data, itsBytes, size);        
        itsBytes += size;
        byteSize += size;
      }

    std::streamsize getSize() {
        return byteSize;
    }

    private:
      char* itsBytes;
      std::streamsize byteSize = 0;
  };

  // ######################################################################
  // Common BinaryArchive serialization functions

  //! Saving for POD types to binary
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  CEREAL_SAVE_FUNCTION_NAME(BytesOutputArchive & ar, T const & t)
  {
    ar.saveBytes(std::addressof(t), sizeof(t));
  }

  //! Loading for POD types from binary
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  CEREAL_LOAD_FUNCTION_NAME(BytesInputArchive & ar, T & t)
  {
    ar.loadBytes(std::addressof(t), sizeof(t));
  }

  //! Serializing NVP types to binary
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(BytesInputArchive, BytesOutputArchive)
  CEREAL_SERIALIZE_FUNCTION_NAME( Archive & ar, NameValuePair<T> & t )
  {
    ar( t.value );
  }

  //! Serializing SizeTags to binary
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(BytesInputArchive, BytesOutputArchive)
  CEREAL_SERIALIZE_FUNCTION_NAME( Archive & ar, SizeTag<T> & t )
  {
    ar( t.size );
  }

  //! Saving binary data
  template <class T> inline
  void CEREAL_SAVE_FUNCTION_NAME(BytesOutputArchive & ar, BinaryData<T> const & bd)
  {
    ar.saveBytes( bd.data,  static_cast<std::streamsize>( bd.size ));
  }

  //! Loading binary data
  template <class T> inline
  void CEREAL_LOAD_FUNCTION_NAME(BytesInputArchive & ar, BinaryData<T> & bd)
  {
    ar.loadBytes(bd.data,  static_cast<std::streamsize>( bd.size ));
  }
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::BytesOutputArchive)
CEREAL_REGISTER_ARCHIVE(cereal::BytesInputArchive)

// tie input and output archives together
CEREAL_SETUP_ARCHIVE_TRAITS(cereal::BytesInputArchive, cereal::BytesOutputArchive)

#endif // CEREAL_ARCHIVES_BINARY_HPP_
