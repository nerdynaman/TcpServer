#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"
#include "linkedList.hh"
#include <algorithm>
#include <cstdint>
#include <deque>
#include <iostream>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    LL _localBuffer;  //!< The buffer of substrings
    size_t _capacity;  //!< The capacity of _localBuffer + _output
    size_t _unassembledBytes;  //!< The number of bytes in the buffer
    // size_t _bufferEmpty;  //!< The number of bytes that can be pushed in the local buffer
    ByteStream _output;  //!< The reassembled in-order byte stream
    int _ackIndex;  //!< The index of the next interested substring
    int eofACK;

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);
    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    // ! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;

    //! The acknowledge index of the stream, i.e., the index of the next interested substring
    size_t ack_index() const;
    void writeBack();
    // string getByteStream();
    string getByteStream(){
      cout << "output: " << _output.bytes_written() << endl;
      cout << "unassembled: " << _unassembledBytes << endl;
      cout << "bufferSize " << _output.buffer_size() << endl;
      cout << "peekOutput " << _output.peek_output(10) << endl;
      cout << "bytes written " << _output.bytes_written() << endl;
      cout << "eof " << _output.eof() << endl;
      return "";
    }
    void read(){
      _output.read(1);
    }

};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
