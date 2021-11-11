#include <zinx.h>
#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include <string>
#include "AOIWorld.h"

using namespace std;

// AOITest
class myPlayer
    :public Player 
{
public:
    myPlayer(int _x, int _y, std::string _name)
        :x(_x), y(_y), name(_name) {}
    int x, y;
    std::string name;

    // 通过 Player 继承
    virtual int GetX() override
    {
        return x;
    }
    virtual int GetY() override
    {
        return y;
    }
};

int main()
{
    /*
    // protobuf test
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(1);
    pmsg->set_username("llllk");
    GameMsg gm(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    auto output = gm.serialize();
    for (auto byte : output) {
        printf("%02x", byte);
    }
    puts("");

    char buff[] = { 0x08,0x01,0x12,0x05,0x6c,0x6c,0x6c,0x6c,0x6b };
    string input(buff, sizeof(buff));

    auto ingm = GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, input);
    cout << dynamic_cast<pb::SyncPid*>(ingm.getMsg())->username() << endl;
    cout << dynamic_cast<pb::SyncPid*>(ingm.getMsg())->pid() << endl;
    */

    /*
    // AOITest
    AOIWorld w(20, 200, 50, 230, 6, 6);
    myPlayer p1(60, 107, "p1");
    myPlayer p2(91, 118, "p2");
    myPlayer p3(147, 133, "p3");
    w.AddPlayer(&p1);
    w.AddPlayer(&p2);
    w.AddPlayer(&p3);
    auto srd_list = w.GetSrdPlayers(&p1);
    for (auto single : srd_list) {
        cout << dynamic_cast<myPlayer*>(single)->name << endl;
    }
    */
    

    ZinxKernel::ZinxKernelInit();
    // 添加监听通道
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GenConnFact())));
    ZinxKernel::Zinx_Run();

    ZinxKernel::ZinxKernelFini();
    return 0;
}