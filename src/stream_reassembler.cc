#include "stream_reassembler.hh"
// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity):_output(capacity), _localBuffer(){
    _capacity = capacity;
    _ackIndex = 0;
    _unassembledBytes = 0;
    // _bufferEmpty = capacity;
}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
    // store the index of the last byte written to the _output, to check if next segment to be pushed is in order
    // first we will write the packet to the local buffer at its correct position then we will iterate over local buffer to write to output,
    // to check if packets from _ackIndex are present and then write them
    // if the packet is in order, we will write it to the output and update the _ackIndex
    LL *_localBufferCopy = &_localBuffer;
    if (index < _ackIndex || (_capacity - _output.bytes_written() - _unassembledBytes) < data.length()){
        // cout << "return 1 " << _ackIndex << " " << index << " " << _capacity << " " << _output.bytes_written() << " " << _unassembledBytes << endl;
        // if the packet is already written to the output or can't be accomodated in local buffer, then we will ignore it
        return;
    }
    LL *newPacket = new LL(index, data);
    while(_localBufferCopy->getNext() != NULL){
        if(_localBufferCopy->getNext()->getPacketIndex() > index){
            break;
        }
        _localBufferCopy = _localBufferCopy->getNext();
    }
    _localBufferCopy->addNext(newPacket);
    _unassembledBytes += data.length();
    // _bufferEmpty -= data.length();
    // now we will iterate over the local buffer to write to output
    _localBufferCopy = &_localBuffer;
    while(_localBufferCopy->getNext() != NULL){
        if(_localBufferCopy->getNext()->getPacketIndex() == _ackIndex){
            int bytesWritten = _output.write(_localBufferCopy->getNext()->getData());
            // update the _ackIndex
            _ackIndex += bytesWritten;
            // update the _unassembledBytes
            _unassembledBytes -= bytesWritten;
            // below block is not required right now as total capacity is comprises of _output + _localBuffer,
            // so if data is present in _localBuffer, it can be added in _output as well
            // if (bytesWritten < _localBufferCopy.getNext()->getData().length()){
            //     // update the packet in the local buffer if it has not been written completely
            //     _localBufferCopy.getNext()->setData(_localBufferCopy.getNext()->getData().substr(bytesWritten));
            //     _localBufferCopy.getNext()->setPacketIndex(_localBufferCopy.getNext()->getPacketIndex() + bytesWritten);
            //     break;
            // }
            // delete the node from the local buffer if it has been written completely
            if (_localBufferCopy->getNext()->getNext()==NULL){
                _localBufferCopy->setNext(NULL);
                break;
            }
            _localBufferCopy->setNext(_localBufferCopy->getNext()->getNext());
        }
        else{
            break;
        }
    }
    // if packet has eof then in _output write eof
    if(eof){
        _output.end_input();
    }
}
size_t StreamReassembler::unassembled_bytes() const { return _unassembledBytes; }

bool StreamReassembler::empty() const { return _unassembledBytes==0 ? true:false; }

size_t StreamReassembler::ack_index() const { return _ackIndex; }

// string StreamReassembler::getByteStream(){
//     return _output.peek_output(100);
// }
