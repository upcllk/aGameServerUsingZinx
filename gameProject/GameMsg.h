#pragma once
#include <zinx.h>
#include <google/protobuf/message.h>
#include <list>
#include "msg.pb.h"

class GameMsg :
    public UserData
{
public:
    enum MSG_TYPE {
        MSG_TYPE_LOGIN_ID_NAME = 1,
        MSG_TYPE_CHAT_CONTENT = 2,
        MSG_TYPE_NEW_POSITION = 3,
        MSG_TYPE_BROADCAST = 200,
        MSG_TYPE_LOGOFF_ID_NAME = 201,
        MSG_TYPE_SRD_POSITION = 202
    };

private:
    enum MSG_TYPE m_enMsgType;
    // �ͻ��˵�������Ϣ
    google::protobuf::Message * m_pMsg = nullptr;

public:
    // ��֪��Ϣ���ݣ�������Ϣ����
    GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg);
    // �ֽ���Ҳ����ת��Ϊ protobuf ��Ϣ�ṹ
    GameMsg(MSG_TYPE _type, std::string _stream);
    google::protobuf::Message* getMsg();
    // ���л�����Ϣ
    std::string serialize();
    int getMsgType();
};

class MultiMsg
    :public UserData
{
public:
    std::list<GameMsg*> m_Msgs;
};

