#pragma once
#include <ZinxTCP.h>
#include "GameProtocol.h"

class GameChannel :
    public ZinxTcpData
{
private:
    GameProtocol* m_proto = nullptr;
public:
    GameChannel(int _fd);
    ~GameChannel();
    void setProtocol(GameProtocol* _proto);
    // ͨ�� ZinxTcpData �̳�
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class GenConnFact :
    public IZinxTcpConnFact
{
    // ͨ�� IZinxTcpConnFact �̳�
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};

