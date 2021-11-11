#include "GameRole.h"
#include "AOIWorld.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "GameChannel.h"
#include <algorithm>
#include <random>

// 创建游戏世界全局对象
static AOIWorld world(0, 400, 0, 400, 20, 20);

static std::default_random_engine random_engine(time(NULL));

GameRole::GameRole()
{
    m_szName = "Klee";
    m_x = 100 + random_engine() % 50;
    m_z = 100 + random_engine() % 50;
}

GameRole::~GameRole()
{
}

GameMsg* GameRole::CreateIDNameLogin()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pMsg);
    return pRet;
}

GameMsg* GameRole::CreateSrdPlayers()
{
    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    auto srd_list = world.GetSrdPlayers(this);  // list<Player*>

    // std::cout << "srd players : " << srd_list.size() << std::endl;

    for (auto single : srd_list) {  // Player*
        auto pPlayer = pMsg->add_ps();  
        auto pRole = dynamic_cast<GameRole*>(single);   
        pPlayer->set_pid(pRole->m_iPid);
        pPlayer->set_username(pRole->m_szName);
        // 设置子消息的子消息
        auto pPosition = pPlayer->mutable_p();
        pPosition->set_x(pRole->m_x);
        pPosition->set_y(pRole->m_y);
        pPosition->set_z(pRole->m_z);
        pPosition->set_v(pRole->m_v);
    }
    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pret;
}

GameMsg* GameRole::CreateSelfPos()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);
    // 客户端规定， 2 为 Pos 消息， 1 为 Content 消息
    pMsg->set_tp(2);
    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(m_x);
    pPosition->set_y(m_y);
    pPosition->set_z(m_z);
    pPosition->set_v(m_v);

    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pret;
}

GameMsg* GameRole::CreateIDNameLogoff()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pMsg);
    return pRet;
}

GameMsg* GameRole::CreateTalkBroadCast(std::string _content)
{
    pb::BroadCast* pmsg = new pb::BroadCast();
    pmsg->set_pid(m_iPid);
    pmsg->set_username(m_szName);
    // 客户端规定， 2 为 Pos 消息， 1 为 Content 消息
    pmsg->set_tp(1);
    pmsg->set_content(_content);

    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pmsg);
    return pRet;
}

void GameRole::ProcTalkMsg(std::string _content)
{
    std::list<Irole*> all_role_list = ZinxKernel::Zinx_GetAllRole();
    // auto pMsg = CreateTalkBroadCast(content);
    for (auto pRole : all_role_list) {
        auto pGameRole = dynamic_cast<GameRole*> (pRole);
        auto pMsg = CreateTalkBroadCast(_content);
        ZinxKernel::Zinx_SendOut(*pMsg, *(pGameRole->m_pProto));
    }
}

void GameRole::ProcMoveMsg(int _x, int _y, int _z, int _v)
{
    /*
    1、跨网格处理
    2、广播新位置给周围玩家
    */
    // 获取原来的邻居 s1
    auto srd_list_pre = world.GetSrdPlayers(this);
    // 摘除旧格子, 更新坐标，获取新邻居 s2，添加新格子完成可能的格子更新
    world.DelPlayer(this);
    m_x = _x;
    m_y = _y;
    m_z = _z;
    m_v = _v;
    world.AddPlayer(this);
    auto srd_list_cur = world.GetSrdPlayers(this);
    // 遍历 s2， 若元素不属于 s1， 视野出现
    for (auto single_player : srd_list_cur) {
        if (find(srd_list_pre.begin(), srd_list_pre.end(), single_player) == srd_list_pre.end()) {
            // 视野出现
            ViewAppear(dynamic_cast<GameRole*> (single_player));
        }
    }

    // 遍历 s1， 若元素不属于 s2， 视野消失
    for (auto single_player : srd_list_pre) {
        if (find(srd_list_cur.begin(), srd_list_cur.end(), single_player) == srd_list_cur.end()) {
            // 视野消失
            ViewLost(dynamic_cast<GameRole*> (single_player));
        }
    }

    auto srd_list = world.GetSrdPlayers(this);

    // 向周围玩家广播更新消息
    for (auto singleRole : srd_list) {
        // 创建待发送消息
        pb::BroadCast* pMsg = new pb::BroadCast();
        pMsg->set_pid(this->m_iPid);
        pMsg->set_username(this->m_szName);
        auto pPos = pMsg->mutable_p();
        pPos->set_x(_x);
        pPos->set_y(_y);
        pPos->set_z(_z);
        pPos->set_v(_v);
        pMsg->set_tp(4);
        GameMsg* gameMsg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
        // 这个 gameMsg 应该不会变化但是放在循环外面就不能同步新位置信息甚至段错误
        // 上面的聊天 msg 也是这样 堆对象 ？
        auto pRole = dynamic_cast<GameRole*> (singleRole);
        /*
        //向外发送数据，将参数1( 堆 对 象)指定的用户数据通过参数2指定的协议对象发出
        static void Zinx_SendOut(UserData & _oUserData, Iprotocol & _oProto);
        */
        ZinxKernel::Zinx_SendOut(*gameMsg, *(pRole->m_pProto));
    }
}

void GameRole::ViewAppear(GameRole* _pRole)
{
    // _pRole 向 this 发 200 消息
    auto pMsg = _pRole->CreateSelfPos();
    ZinxKernel::Zinx_SendOut(*pMsg, *m_pProto);
    // this 向 _pRole 发 200 消息
    pMsg = this->CreateSelfPos();
    ZinxKernel::Zinx_SendOut(*pMsg, *(_pRole->m_pProto));

}

void GameRole::ViewLost(GameRole* _pRole)
{
    // _pRole 向 this 发 200 消息
    auto pMsg = _pRole->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pMsg, *m_pProto);
    // this 向 _pRole 发 200 消息
    pMsg = this->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pMsg, *(_pRole->m_pProto));
}

bool GameRole::Init()
{
    // 设置玩家 ID 为当前连接的 fd
    m_iPid = this->m_pProto->getChannel()->GetFd();

    bool bRet = false;
    // 添加玩家（自己）到游戏世界
    bRet = world.AddPlayer(this);
    if (bRet == true) {
        // 发送 ID 和名称
        auto pmsg = CreateIDNameLogin();
        // std::cout << pmsg->getMsg()->Utf8DebugString() << std::endl;
        ZinxKernel::Zinx_SendOut(*pmsg, *(this->m_pProto));
        pmsg = CreateSrdPlayers();
        // std::cout << pmsg->getMsg()->Utf8DebugString() << std::endl;
        ZinxKernel::Zinx_SendOut(*pmsg, *(this->m_pProto));
        // 向周围玩家发送自己的位置
        auto srd_list = world.GetSrdPlayers(this);
        // std::cout << "in Init : srd_list.size() = " << srd_list.size() << std::endl;
        for (auto single : srd_list) {
            pmsg = CreateSelfPos();
            auto pRole = dynamic_cast<GameRole*> (single);
            // 发给其他玩家的协议层
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));
        }
    }
    return bRet;
}

// 处理相关用户请求
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    // test
    GET_REF2DATA(MultiMsg, input, _poUserData); 
    for (auto single : input.m_Msgs) {
        std::cout << "type is " << single->getMsgType() << std::endl;
        std::cout << single->getMsg()->Utf8DebugString() << std::endl;
        switch (single->getMsgType()) {
        case GameMsg::MSG_TYPE_CHAT_CONTENT:
            //google::protobuf::Message --> pb::Talk
            ProcTalkMsg((dynamic_cast<pb::Talk*> (single->getMsg()))->content());
            break;
        case GameMsg::MSG_TYPE_NEW_POSITION: {
            // 这里不加 {} 限定作用域会报错 jumo to case label , 或者 newPos 在外面定义
            pb::Position* newPos = dynamic_cast<pb::Position*> (single->getMsg());
            ProcMoveMsg(newPos->x(), newPos->y(), newPos->z(), newPos->v());
        }
            break;
        default:
            break;
        }
    }

    return nullptr;
}

void GameRole::Fini()
{
    // 向周围玩家发送下线消息
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list) {
        auto pMsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*> (single);
        // 发给其他玩家的协议层`
        ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_pProto));
    }
    world.DelPlayer(this);
}

void GameRole::setProtocol(GameProtocol* _pProtocol)
{
    this->m_pProto = _pProtocol;
}

int GameRole::GetX()
{
    return (int)m_x;
}

int GameRole::GetY()
{
    return (int)m_z;
}
