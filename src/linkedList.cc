#include "linkedList.hh"
#include <iostream>
LL::LL()
{
    this->packetIndex = -1;
    this->next = NULL;
    this->data = "";
    this->eof = 0;
    // this->segmentIndex = -1;
}
LL::LL(int packetIndex, std::string data, int eof)
{
    this->packetIndex = packetIndex;
    this->next = NULL;
    this->data = data;
    this->eof = eof;
}
void LL::setNext(LL *next)
{
    this->next = next;
}
void LL::addNext(LL *next)
{
    LL *temp = this->next;
    this->next = next;
    next->next = temp;
}
void LL::setData(string data)
{
    this->data = data;
}
void LL::setPacketIndex(int packetIndex)
{
    this->packetIndex = packetIndex;
}
int LL::getPacketIndex()
{
    return this->packetIndex;
}
int LL::getEOF(){
    return this->eof;
}
string LL::getData()
{
    return this->data;
}
LL *LL::getNext()
{
    return this->next;
}