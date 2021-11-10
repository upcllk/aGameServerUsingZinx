#include "GameChannel.h"
#include "GameRole.h"

GameChannel::GameChannel(int _fd) :ZinxTcpData(_fd)
{

}

GameChannel::~GameChannel()
{
    if (m_proto != nullptr) {
        ZinxKernel::Zinx_Del_Proto(*m_proto);
        delete m_proto;
    }
}

void GameChannel::setProtocol(GameProtocol* _proto)
{
    this->m_proto = _proto;
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& _oInput)
{
    // 返回一个协议对象 每个通道绑定一个协议对象，每个协议对象只会给单一通道发送数据
    return m_proto;
}

ZinxTcpData* GenConnFact::CreateTcpDataChannel(int _fd)
{
    // 创建 TCP 通道对象, 创建协议对象并将两者绑定, 将协议对象添加到 Kernel
    auto pChannel = new GameChannel(_fd);
    auto pProtocol = new GameProtocol();
    pChannel->setProtocol(pProtocol);
    // 双向绑定
    pProtocol->setChannel(pChannel);
    // 创建玩家对象
    auto pRole = new GameRole();
    // 绑定协议对象和玩家对象
    pProtocol->setGameRole(pRole);
    pRole->setProtocol(pProtocol);

    // 添加协议对象，要求对象为堆对象，该函数仅有内存管理的作用，建议开发者将协议对象添加到zinxkernel存储
    ZinxKernel::Zinx_Add_Proto(*pProtocol); 
    // 添加玩家对象
    ZinxKernel::Zinx_Add_Role(*pRole);

    return pChannel; 
}
