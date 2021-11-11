#pragma once
#include <zinx.h>
#include <list>
#include <vector>

class ZinxTimerChannel :
    public Ichannel
{
private:
    int m_Timerfd = -1;
public:
    // 通过 Ichannel 继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class output_hello :
    public AZinxHandler
{
    // 通过 AZinxHandler 继承
    virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;
    virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;
};

/*由调用者继承决定超时任务和超时时间*/
class TimerOutProc {
public:
    virtual void Proc() = 0;
    virtual int GetTimeSec() = 0;
    // 所剩圈数
    int iCount = -1;
};

const int WHELLSIZE = 10;
// 单例
class TimeOutMng :
    public AZinxHandler
{
    // 通过 AZinxHandler 继承
private:
    std::vector<std::list<TimerOutProc*>> m_timer_whell;
    int m_cur_index = 0;
    // std::list<TimerOutProc*>m_task_list;
    static TimeOutMng* single;
    TimeOutMng();
public:
    /*处理超时事件，遍历所有的任务，计数 -1*/
    virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;
    virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;
    void AddTask(TimerOutProc* _ptask);
    void DelTask(TimerOutProc* _ptask);
    static TimeOutMng* GetInstance() {
        return single;
    }
};