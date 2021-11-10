#include "GameRole.h"
#include "AOIWorld.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "GameChannel.h"

// ������Ϸ����ȫ�ֶ���
static AOIWorld world(0, 400, 0, 400, 20, 20);

GameRole::GameRole()
{
    m_szName = "Klee";
    m_x = 100;
    m_y = 100;
}

GameRole::~GameRole()
{
}

GameMsg* GameRole::CreateIDNameLogin()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(m_iPid);
    pmsg->set_username(m_szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return pRet;
}

GameMsg* GameRole::GetSrdPlayers()
{
    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    auto srd_list = world.GetSrdPlayers(this);  // list<Player*>

    std::cout << "srd players : " << srd_list.size() << std::endl;

    for (auto single : srd_list) {  // Player*
        auto pPlayer = pMsg->add_ps();  
        auto pRole = dynamic_cast<GameRole*>(single);   
        pPlayer->set_pid(pRole->m_iPid);
        pPlayer->set_username(pRole->m_szName);
        // ��������Ϣ������Ϣ
        auto pPosition = pPlayer->mutable_p();
        pPosition->set_x(pRole->m_x);
        pPosition->set_y(pRole->m_y);
        pPosition->set_z(pRole->m_z);
        pPosition->set_v(pRole->m_v);
    }
    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pret;
}

GameMsg* GameRole::SendSelfPos()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);
    // �ͻ��˹涨�� 2 Ϊ Pos ��Ϣ�� 1 Ϊ Content ��Ϣ
    pMsg->set_tp(2);
    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(m_x);
    pPosition->set_y(m_z);
    pPosition->set_z(m_y);
    pPosition->set_v(m_v);

    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pret;
}

GameMsg* GameRole::CreateIDNameLogoff()
{
    return GameRole::CreateIDNameLogin();
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(m_iPid);
    pmsg->set_username(m_szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pmsg);
    return pRet;
}

bool GameRole::Init()
{
    // ������� ID Ϊ��ǰ���ӵ� fd
    m_iPid = this->m_pProto->getChannel()->GetFd();

    bool bRet = false;
    // �����ң��Լ�������Ϸ����
    bRet = world.AddPlayer(this);
    if (bRet == true) {
        // ���� ID ������
        auto pmsg = CreateIDNameLogin();
        // std::cout << pmsg->getMsg()->Utf8DebugString() << std::endl;
        ZinxKernel::Zinx_SendOut(*pmsg, *(this->m_pProto));
        pmsg = GetSrdPlayers();
        // std::cout << pmsg->getMsg()->Utf8DebugString() << std::endl;
        ZinxKernel::Zinx_SendOut(*pmsg, *(this->m_pProto));
        // ����Χ��ҷ����Լ���λ��
        auto srd_list = world.GetSrdPlayers(this);
        std::cout << "in Init : srd_list.size() = " << srd_list.size() << std::endl;
        for (auto single : srd_list) {
            pmsg = SendSelfPos();
            auto pRole = dynamic_cast<GameRole*> (single);
            // ����������ҵ�Э���
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));
        }
    }
    return bRet;
}

// ��������û�����
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    // test
    GET_REF2DATA(MultiMsg, input, _poUserData);
    for (auto single : input.m_Msgs) {
        std::cout << "type is " << single->getMsgType() << std::endl;
        std::cout << single->getMsg()->Utf8DebugString() << std::endl;
    }

    return nullptr;
}

void GameRole::Fini()
{
    // ����Χ��ҷ���������Ϣ
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list) {
        auto pMsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*> (single);
        // ����������ҵ�Э���`
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
