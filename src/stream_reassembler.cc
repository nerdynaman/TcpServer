#include "stream_reassembler.hh"
#include <cstring>
#include <climits>
// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity):_output(capacity), _localBuffer(){
    _capacity = capacity;
    _ackIndex = 0;
    _unassembledBytes = 0;
    eofACK = 0;
}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
    writeBack();
    LL *_localBufferCopy = &_localBuffer;
    string dataNew = data;
    int indexNew = index;
    if ((indexNew+dataNew.size()-1 < _ackIndex)){
        if (eof){
            eofACK=1;
        }
        if (eofACK && empty()){
            _output.end_input();
        }
        return;
    } 
    else {
        if (dataNew.size() == 0) {
            if (eof){
                eofACK=1;
            }
            writeBack();
            return;
        }
        if (indexNew < _ackIndex) {
            // Discard the overlapping data in the new packet
            dataNew = dataNew.substr(_ackIndex-indexNew, dataNew.size()-(_ackIndex-indexNew));
            indexNew = _ackIndex;
        }
        // Discard the overlapping data in the buffer
        LL *prevPacket = &_localBuffer;
        LL *currPacket = &_localBuffer;
        currPacket = currPacket->getNext();
        while (currPacket != NULL )  {
            if ( indexNew < currPacket->getPacketIndex() + currPacket->getData().size()) {
                if (indexNew + dataNew.size()-1 < currPacket->getPacketIndex()) {
                    break;
                }
                if (indexNew+dataNew.size() <= currPacket->getPacketIndex() + currPacket->getData().size()) {
                    if (indexNew>=currPacket->getPacketIndex()){
                        if (eof){
                            eofACK=1;
                        }
                        writeBack();
                        return;
                    }
                    // Discard the overlapping data in the buffer
                    int discardByteSize = indexNew+dataNew.size() - currPacket->getPacketIndex();
                    if (discardByteSize < currPacket->getData().size()){
                        _unassembledBytes -= discardByteSize;
                        currPacket->setData(currPacket->getData().substr(discardByteSize, currPacket->getData().size()-discardByteSize));
                        currPacket->setPacketIndex(currPacket->getPacketIndex() + discardByteSize);
                    }
                    else{
                        _unassembledBytes -= currPacket->getData().size();
                        prevPacket->setNext(currPacket->getNext());
                        currPacket = prevPacket;
                    }
                }
                else{
                    int discardByteSize = currPacket->getPacketIndex() + currPacket->getData().size() -indexNew;
                    if (discardByteSize >= currPacket->getData().size()){
                        _unassembledBytes -= currPacket->getData().size();
                        prevPacket->setNext(currPacket->getNext());
                        currPacket = prevPacket;
                    }
                    else{
                        currPacket->setData(currPacket->getData().substr(0, currPacket->getData().size()-discardByteSize));
                        _unassembledBytes -= discardByteSize;
                    }
                }
            } 
            prevPacket = currPacket;
            currPacket = currPacket->getNext();
        }
        // Add the new packet to the buffer
        if ((_unassembledBytes + dataNew.length() > _capacity) && (_capacity-_unassembledBytes > 0)) {
            dataNew = dataNew.substr(0, _capacity - _unassembledBytes);
            indexNew = _ackIndex + _unassembledBytes;
        }

        _localBufferCopy = &_localBuffer;
        if (_capacity-_unassembledBytes <= 0 && indexNew == _ackIndex){
            // keep _capacity - dataNew.size() bytes in the buffer and remove all other
            int maxCap = _capacity - dataNew.size();
            int localCap = 0;
            while (_localBufferCopy->getNext() != NULL){
                localCap += _localBufferCopy->getNext()->getData().size();
                if (localCap > maxCap){
                    // remove extra bytes
                    _unassembledBytes = maxCap;
                    _localBufferCopy->getNext()->setData(_localBufferCopy->getNext()->getData().substr(0, _localBufferCopy->getNext()->getData().size()-(localCap-maxCap)));
                    _localBufferCopy->getNext()->setNext(NULL);
                    break;
                }
                _localBufferCopy = _localBufferCopy->getNext();
            }
        }

        if (_capacity-_unassembledBytes <= 0 && indexNew != _ackIndex){
            if (eof){
                eofACK=1;
            }
            writeBack();
            return;
        }
        else{
            _localBufferCopy = &_localBuffer;
            while (_localBufferCopy->getNext() != NULL){
                if (_localBufferCopy->getNext()->getPacketIndex() > indexNew){
                    break;
                }
                _localBufferCopy = _localBufferCopy->getNext();
            }
            LL *newPacket = new LL(indexNew, dataNew, eof);
            if (eof){
                eofACK =1;
            }
            if (_localBufferCopy->getNext() == NULL){
                _localBufferCopy->setNext(newPacket);
            }
            else{
                _localBufferCopy->addNext(newPacket);
            }
            _unassembledBytes += dataNew.length();
        }
    }
    writeBack();
}
void StreamReassembler::writeBack(){
    LL* _localBufferCopy = &_localBuffer;
    while(_localBufferCopy->getNext() != NULL){
        if(_localBufferCopy->getNext()->getPacketIndex() == _ackIndex){
            int bytesWritten = _output.write(_localBufferCopy->getNext()->getData());
            _ackIndex += bytesWritten;
            _unassembledBytes -= bytesWritten;
            if (bytesWritten < (_localBufferCopy->getNext()->getData().length())){
                _localBufferCopy->getNext()->setData(_localBufferCopy->getNext()->getData().substr(bytesWritten));
                _localBufferCopy->getNext()->setPacketIndex(_localBufferCopy->getNext()->getPacketIndex() + bytesWritten);
                break;
            }
            else{
                _localBufferCopy->setNext(_localBufferCopy->getNext()->getNext());
            }
        }
        else{
            _localBufferCopy = _localBufferCopy->getNext();
        }
    }
    if (empty() && eofACK){
        _output.end_input();
    }
}
size_t StreamReassembler::unassembled_bytes() const { return _unassembledBytes; }

bool StreamReassembler::empty() const { return _unassembledBytes==0 ? true:false; }

size_t StreamReassembler::ack_index() const { return _ackIndex; }
