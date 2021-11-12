#include <zinx.h>
#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include <string>
#include "AOIWorld.h"
#include "ZinxTimer.h"
#include "RandomName.h"

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

extern RandomName random_name;

void daemonlize() {
    /*
    1、fork
    2、父进程退出
    3、子进程设置回话 ID
    4、子进程设置指向路径
    5、子进程重定向三个文件描述符
    */
    int pid = fork();
    if (pid < 0) {
        exit(-1);
    }
    if (pid > 0) {
        // 父进程退出
        exit(0);
    }
    setsid();
    // 默认当前路径
    int nullfd = open("/dev/null", O_RDWR);
    if (nullfd >= 0) {
        dup2(nullfd, STDIN_FILENO);
        dup2(nullfd, STDOUT_FILENO);
        dup2(nullfd, STDERR_FILENO);
        close(nullfd);
    }

    //设置进程监控
    while (1)
    {
        pid = fork();
        if (pid < 0) {
            exit(-1);
        }

        // 父进程等子进程退出
        if (pid > 0) {
            int status = 0;
            wait(&status);
            if (status == 0) {
                // 正常退出(定时器)
                exit(0);
            }
        }
        // 子进程跳出循环执行游戏业务
        else {
            break;
        }
    }
}


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
    daemonlize();
    random_name.LoadFile();
    ZinxKernel::ZinxKernelInit();
    // 添加监听通道
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GenConnFact())));
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));
    ZinxKernel::Zinx_Run();

    ZinxKernel::ZinxKernelFini();
    return 0;
}