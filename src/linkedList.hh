#include<string>
using namespace std;
class LL
{
private:
    int packetIndex;
    int segmentIndex;
    string data;
    LL *next;    
public:
    LL();
    LL(int packetIndex, string data);
    void setNext(LL *next);
    void addNext(LL *next);
    void setData(string data);
    void setPacketIndex(int packetIndex);
    int getPacketIndex();
    string getData();
    LL *getNext();
};
