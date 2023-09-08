#include "byte_stream.hh"

#include <algorithm>

using namespace std;

ByteStream::ByteStream(const size_t capa) {
  _capacity = capa;
};


size_t ByteStream::write(const string &data) {
  int writable = _capacity - _buffer.size();
  writable = min(writable, (int)data.size());
  for (int i = 0; i < writable; i++) {
    _buffer.push_back(data[i]);
    _bytesWritten+=1;
  }
  return writable;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
  string result = "";
  // const char* ch = &_buffer.front();
  int peekSize = (len>_buffer.size()) ? _buffer.size():len;
  for(int i = 0; i < peekSize ;i++){
    result.push_back(_buffer[i]);
  }
  // cout << result << " "<<len << " " << peekSize<< "\n";
  return result;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    int popSize = (len>_buffer.size()) ? _buffer.size():len;
    for(int i = 0; i < popSize; i++){
    _buffer.pop_front();
    _bytesRead++;
  }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
  string result = ByteStream::peek_output(len);
  ByteStream::pop_output(len);
  return result;
}

void ByteStream::end_input() {_endInput = true;}

bool ByteStream::input_ended() const { return _endInput;}

size_t ByteStream::buffer_size() const {return _buffer.size(); }

bool ByteStream::buffer_empty() const {return _buffer.empty();}

bool ByteStream::eof() const { return _endInput && _buffer.empty();}

size_t ByteStream::bytes_written() const { return _bytesWritten; }

size_t ByteStream::bytes_read() const { return _bytesRead;  }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }