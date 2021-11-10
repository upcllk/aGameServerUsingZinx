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
    // 客户端的请求信息
    google::protobuf::Message * m_pMsg = nullptr;

public:
    // 已知消息内容，创建消息对象
    GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg);
    // 字节流也可由转化为 protobuf 消息结构
    GameMsg(MSG_TYPE _type, std::string _stream);
    google::protobuf::Message* getMsg();
    // 序列化本消息
    std::string serialize();
    int getMsgType();
};

class MultiMsg
    :public UserData
{
public:
    std::list<GameMsg*> m_Msgs;
};

