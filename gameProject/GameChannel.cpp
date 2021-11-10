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
    // ����һ��Э����� ÿ��ͨ����һ��Э�����ÿ��Э�����ֻ�����һͨ����������
    return m_proto;
}

ZinxTcpData* GenConnFact::CreateTcpDataChannel(int _fd)
{
    // ���� TCP ͨ������, ����Э����󲢽����߰�, ��Э�������ӵ� Kernel
    auto pChannel = new GameChannel(_fd);
    auto pProtocol = new GameProtocol();
    pChannel->setProtocol(pProtocol);
    // ˫���
    pProtocol->setChannel(pChannel);
    // ������Ҷ���
    auto pRole = new GameRole();
    // ��Э��������Ҷ���
    pProtocol->setGameRole(pRole);
    pRole->setProtocol(pProtocol);

    // ���Э�����Ҫ�����Ϊ�Ѷ��󣬸ú��������ڴ��������ã����鿪���߽�Э�������ӵ�zinxkernel�洢
    ZinxKernel::Zinx_Add_Proto(*pProtocol); 
    // �����Ҷ���
    ZinxKernel::Zinx_Add_Role(*pRole);

    return pChannel; 
}
