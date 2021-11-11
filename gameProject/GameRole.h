#pragma once
#include <zinx.h>
#include "AOIWorld.h"
#include "GameMsg.h"

class GameProtocol;
class GameRole :
    public Irole, public Player
{
public:
    GameRole();
    ~GameRole();
private:
    GameProtocol* m_pProto = nullptr;

private:
    float m_x = 0;
    float m_y = 0;
    float m_z = 0;
    float m_v = 0;
    int m_iPid = 0;
    std::string m_szName;
    GameMsg* CreateIDNameLogin();
    GameMsg* CreateSrdPlayers();
    GameMsg* CreateSelfPos();
    GameMsg* CreateIDNameLogoff();
    GameMsg* CreateTalkBroadCast(std::string _content);

    void ProcTalkMsg(std::string _content);
    void ProcMoveMsg(int x, int y, int z, int v);
    
    void ViewAppear(GameRole* _pRole);
    void ViewLost(GameRole* _pRole);

public:
    // 通过 Irole 继承
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;
    void setProtocol(GameProtocol* _pProtocol);

    // 通过 Player 继承
    virtual int GetX() override;
    virtual int GetY() override;
};

