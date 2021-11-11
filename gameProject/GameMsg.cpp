#include "GameMsg.h"

GameMsg::GameMsg()
{
}

GameMsg::~GameMsg()
{
	if (m_pMsg != nullptr) {
		delete m_pMsg;
	}
}

GameMsg::GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg)
	:m_enMsgType(_type), m_pMsg(_pMsg)
{
}

GameMsg::GameMsg(MSG_TYPE _type, std::string _stream)
	: m_enMsgType(_type)
{
	// ͨ���򵥹������������Ϣ����
	switch (_type)
	{
	case GameMsg::MSG_TYPE_LOGIN_ID_NAME:
		m_pMsg = new pb::SyncPid();
		break;
	case GameMsg::MSG_TYPE_CHAT_CONTENT:
		m_pMsg = new pb::Talk();
		break;
	case GameMsg::MSG_TYPE_NEW_POSITION:
		m_pMsg = new pb::Position();
		break;
	case GameMsg::MSG_TYPE_BROADCAST:
		m_pMsg = new pb::BroadCast();
		break;
	case GameMsg::MSG_TYPE_LOGOFF_ID_NAME:
		m_pMsg = new pb::SyncPid();
		break;
	case GameMsg::MSG_TYPE_SRD_POSITION:
		m_pMsg = new pb::SyncPlayers();
		break;
	default:
		break;
	}

	// ��������������Ϣ��������
	m_pMsg->ParseFromString(_stream);
}

google::protobuf::Message* GameMsg::getMsg()
{
	return m_pMsg;
}

std::string GameMsg::serialize()
{
	std::string ret;
	m_pMsg->SerializeToString(&ret);
	return ret;
}

int GameMsg::getMsgType()
{
	return m_enMsgType;
}

MultiMsg::MultiMsg()
{
}

MultiMsg::~MultiMsg()
{
	// ��߲�д���������ڴ�й©(Ϊɶ��Ƶ���û��)
	// std::list<GameMsg*> m_Msgs;
	while (!m_Msgs.empty()) {
		GameMsg* tmp = *m_Msgs.begin();
		m_Msgs.remove(tmp);
		if (tmp != nullptr) {
			delete tmp;
		}
	}
}
