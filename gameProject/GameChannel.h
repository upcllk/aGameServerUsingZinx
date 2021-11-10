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
    // 通过 ZinxTcpData 继承
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class GenConnFact :
    public IZinxTcpConnFact
{
    // 通过 IZinxTcpConnFact 继承
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};

