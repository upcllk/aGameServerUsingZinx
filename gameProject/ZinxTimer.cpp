#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <unistd.h>

// 创建定时器文件描述符, 成功 true 失败 false
bool ZinxTimerChannel::Init()
{
    int bRet = false;
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (iFd >= 0) {
        struct itimerspec period = {
            {1,0}, {1,0}
        };
        if (timerfd_settime(iFd, 0, &period, NULL) == 0) {
            bRet = true;
            m_Timerfd = iFd;
        }
    }
    return bRet;
}

// 读取超时次数， 一般就是 1 
bool ZinxTimerChannel::ReadFd(std::string& _input)
{
    bool bRet = false;
    char buf[8] = { 0 };
    if (read(m_Timerfd, buf, sizeof(buf)) == sizeof(buf)) {
        bRet = true;
        _input.assign(buf, sizeof(buf));
    }
    return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
    return false;
}

// 关闭文件描述符
void ZinxTimerChannel::Fini()
{

    close(m_Timerfd);
    m_Timerfd = -1;
}

// 返回当前的定时器 fd
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "timerFd";
}

// 返回处理超时时间的对象
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
    // output_hello* pout_hello = new output_hello();
    // return pout_hello;
    return TimeOutMng::GetInstance();
}

IZinxMsg* output_hello::InternelHandle(IZinxMsg& _oInput)
{
    // 获取输出通道
    auto pChannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
    std::string output = "hello world";
    ZinxKernel::Zinx_SendOut(output, *pChannel);
    return nullptr;
}

AZinxHandler* output_hello::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

TimeOutMng::TimeOutMng() {
    // 创建 10 个齿
    for (int i = 0; i < WHELLSIZE; ++i) {
        std::list<TimerOutProc*> tmp;
        m_timer_whell.push_back(tmp);
    }
}

TimeOutMng* TimeOutMng::single = new TimeOutMng();

IZinxMsg* TimeOutMng::InternelHandle(IZinxMsg& _oInput)
{
    // 超时多少次 防止因为阻塞造成的多次超时
    uint64_t iTimeOutCount = 0;
    GET_REF2DATA(BytesMsg, oBytes, _oInput);
    oBytes.szData.copy((char*)&iTimeOutCount, sizeof(iTimeOutCount), 0);

    while (iTimeOutCount-- > 0) {
        // 移动刻度
        // 当前刻度节点圈数 -1 或 执行超时函数
        m_cur_index = (m_cur_index + 1) % WHELLSIZE;
        std::list<TimerOutProc*> m_cache;
        for (auto ite = m_timer_whell[m_cur_index].begin();
            ite != m_timer_whell[m_cur_index].end(); ) {
            if ((*ite)->iCount <= 0) {
                // 存在安全隐患-  
                // Proc() : TimeOutMng::GetInstance()->DelTask(this);
                // 可以根据缓存的思路，把要执行超时函数的 task 存到一个临时容器， for 循环后统一处理
                // (*ite)->Proc();
                m_cache.push_back(*ite);
                auto tmp = *ite;
                ite = m_timer_whell[m_cur_index].erase(ite);
                TimeOutMng::AddTask(tmp);
            }
            else {
                --(*ite)->iCount;
                ++ite;
            }
        }

        // 统一处理超时任务
        for (auto task : m_cache) {
            task->Proc();
        }
    }
    
    return nullptr;
}

AZinxHandler* TimeOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

void TimeOutMng::AddTask(TimerOutProc* _ptask)
{
    // 计算当前任务设置到哪个齿上 以及 所需圈数
    int index = (_ptask->GetTimeSec() + m_cur_index) % WHELLSIZE;
    _ptask->iCount = _ptask->GetTimeSec() / WHELLSIZE;
    m_timer_whell[index].push_back(_ptask);
    
    // _ptask->iCount = _ptask->GetTimeSec();
    // m_task_list.push_back(_ptask);
}

void TimeOutMng::DelTask(TimerOutProc* _ptask)
{
    // 遍历时间轮删除任务
    // 默认拷贝记得 &
    // auto& -> list<TimerOutProc*>&
    for (auto& gear : m_timer_whell) {
        for (auto task : gear) {
            if (task == _ptask) {
                gear.remove(task);
                return;
            }
        }
    }
    // m_task_list.remove(_ptask);
}
