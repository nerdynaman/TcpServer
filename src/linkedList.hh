#include<string>
using namespace std;
class LL
{
private:
    int packetIndex;
    int segmentIndex;
    int eof;
    string data;
    LL *next;    
public:
    LL();
    LL(int packetIndex, string data, int eof);
    void setNext(LL *next);
    void addNext(LL *next);
    void setData(string data);
    void setPacketIndex(int packetIndex);
    int getPacketIndex();
    int getEOF();
    string getData();
    LL *getNext();
};
