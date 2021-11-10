#pragma once
#include <zinx.h>

// ������ò���ͷ�ļ��� ��ֹѭ������
class GameChannel;
class GameRole;

class GameProtocol :
    public Iprotocol
{
private:
    std::string m_dataCache;
    GameChannel* m_channel = nullptr;
    GameRole* m_role = nullptr;
public:
    GameProtocol();
    ~GameProtocol();
    // ͨ�� Iprotocol �̳�
    virtual UserData* raw2request(std::string _szInput) override;
    virtual std::string* response2raw(UserData& _oUserData) override;
    virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;

    void setChannel(GameChannel* _pChannel);
    void setGameRole(GameRole* _pRole);
    Irole* getGameRole();
    Ichannel* getChannel();
};

