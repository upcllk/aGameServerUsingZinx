#include "GameProtocol.h"
#include "GameMsg.h"
#include "GameChannel.h"
#include "msg.pb.h"
#include "GameRole.h"

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
    if (m_role != nullptr) {
        ZinxKernel::Zinx_Del_Role(*m_role);
        delete m_role;
    }
}

/*  ������ͨ��������ԭʼ���ģ�
    ����ֵ��ת�������Ϣ���� 
    ת����ʽ TCP ճ������ */
UserData* GameProtocol::raw2request(std::string _szInput)
{
    MultiMsg* pRet = new MultiMsg();
    m_dataCache.append(_szInput);
    while (1) {
        if (m_dataCache.size() < 8)
            break;
        int iLength = 0;
        iLength |= m_dataCache[0] << 0;
        iLength |= m_dataCache[1] << 8;
        iLength |= m_dataCache[2] << 16;
        iLength |= m_dataCache[3] << 24;
        int id = 0;
        id |= m_dataCache[4] << 0;
        id |= m_dataCache[5] << 8;
        id |= m_dataCache[6] << 16;
        id |= m_dataCache[7] << 24;
        // ͨ�������ĳ����жϺ��������Ƿ�Ϸ�
        if ((int)m_dataCache.size() - 8 >= iLength) {
            // ����һ���û�����
            GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)id, m_dataCache.substr(8, iLength));
            pRet->m_Msgs.push_back(pMsg);
            m_dataCache.erase(0, 8 + iLength);
        }
        else {
            break;
        }
    }

    // test
    //for (auto single : pRet->m_Msgs) {
    //    /*
    //    * 14:16:18 �������ݣ�{09 00 00 00 01 00 00 00 08 01 12 05 6C 6C 6C 6C 6B }	
    //    * <----------------------------------------->
    //        Pid: 1
    //        Username: "llllk"
    //    */
    //    std::cout << single->getMsg()->Utf8DebugString() << std::endl;
    //}

    //pb::Talk* pmsg = new pb::Talk();
    //pmsg->set_content("hello");
    //GameMsg* pGameMsg = new GameMsg(GameMsg::MSG_TYPE_CHAT_CONTENT, pmsg);
    //ZinxKernel::Zinx_SendOut(*(pGameMsg), *this);

    return pRet;
}

/*  ��������ҵ��㣬�����͵���Ϣ��
    ����ֵ��ת������ֽ�����
    ����ͨ������ */
std::string* GameProtocol::response2raw(UserData& _oUserData)
{
    int iLength = 0;
    int id = 0;
    std::string msgContent;
    GET_REF2DATA(GameMsg, oOutput, _oUserData);
    id = oOutput.getMsgType();
    msgContent = oOutput.serialize();
    iLength = msgContent.size();

    auto pret = new std::string();
    
    pret->push_back((iLength >> 0) & 0xff);
    pret->push_back((iLength >> 8) & 0xff);
    pret->push_back((iLength >> 16) & 0xff);
    pret->push_back((iLength >> 24) & 0xff);
    pret->push_back((id >> 0) & 0xff);
    pret->push_back((id >> 8) & 0xff);
    pret->push_back((id >> 16) & 0xff);
    pret->push_back((id >> 24) & 0xff);
    pret->append(msgContent);

    return pret;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    return m_role;
}

// �������ݷ��͵�ͨ��
Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
    return m_channel;
}

void GameProtocol::setChannel(GameChannel* _pChannel)
{
    this->m_channel = _pChannel;
}

void GameProtocol::setGameRole(GameRole* _pRole)
{
    this->m_role = _pRole;
}

Irole* GameProtocol::getGameRole()
{
    return this->m_role;
}

Ichannel* GameProtocol::getChannel()
{
    return this->m_channel;
}
