#include <vector>
#include <memory>
#include <circle/sched/task.h>
#include <circle/sched/scheduler.h>
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysThreads.h"

namespace SysPlatform {

SysThreads sysThreads;

class ThreadTask : public CTask
{
public:

    ThreadTask() = delete;
    ThreadTask(SysThreads::SysThreadFunction functionToRunIn, void* argIn, int stackSizeIn, void *stackIn)
      // for some reason, using the non-default constructor for CTask results in the task not starting
      //: CTask(stackSize),
      : functionToRun(functionToRunIn), argPtr(argIn), stackSize(stackSizeIn), stack(stackIn)
      {

      }

    void Run() override { functionToRun(argPtr); }
    SysThreads::SysThreadFunction functionToRun;
    void* argPtr;
    int stackSize;
    void* stack = nullptr;
};


struct SysThreads::_impl {
    std::vector<std::shared_ptr<ThreadTask>> threadsVec;
};

SysThreads::SysThreads()
: m_pimpl(std::make_unique<_impl>())
{

}

SysThreads::~SysThreads()
{

}

int SysThreads::addThread(SysThreads::SysThreadFunction p, void * arg, int stackSize, void *stack)
{
    std::shared_ptr<ThreadTask> taskPtr = std::make_shared<ThreadTask>(p, arg, stackSize, stack);
    m_pimpl->threadsVec.push_back(taskPtr);
    return m_pimpl->threadsVec.size()-1;
}

int SysThreads::getState(int id)
{
    if (id >= (int)m_pimpl->threadsVec.size()) { return -1; }
    if (m_pimpl->threadsVec[id]->IsSuspended()) { return SUSPENDED; }
    else { return RUNNING; }
}

int SysThreads::setState(int id, int state) {
    if (id >= (int)m_pimpl->threadsVec.size()) { return -1; }
    if (state == RUNNING) { if (m_pimpl->threadsVec[id]->IsSuspended()) { m_pimpl->threadsVec[id]->Resume(); } }
    else {
        m_pimpl->threadsVec[id]->Suspend();
    }
    return 0;
}

int SysThreads::suspend(int id)
{
    if (id >= (int)m_pimpl->threadsVec.size()) { return -1; }
    m_pimpl->threadsVec[id]->Suspend(); return 0;
}

int SysThreads::restart(int id)
{
    if (id >= (int)m_pimpl->threadsVec.size()) { return -1; }
    m_pimpl->threadsVec[id]->Resume(); return 0;
}

int SysThreads::setTimeSlice(int id, unsigned ticks)
{
    if (id >= (int)m_pimpl->threadsVec.size()) { return SYS_FAILURE; }
    return SYS_SUCCESS;
}

int SysThreads::setTimeSliceUs(int id, unsigned microseconds)
{
    if (id >= (int)m_pimpl->threadsVec.size()) { return SYS_FAILURE; }
    return SYS_SUCCESS;
}

int SysThreads::id() {
    CScheduler* schedPtr = CScheduler::Get();
    if (!schedPtr) { return -1; }

    CTask* taskPtr = schedPtr->GetCurrentTask();
    if (!taskPtr) { return -1; }
    if (m_pimpl->threadsVec.size() < 1) { return -1; }
    for (int i=0; i < (int)m_pimpl->threadsVec.size(); i++) {
        if (taskPtr == m_pimpl->threadsVec[i].get()) {
            return i;
        }
    }
    return -1;
}

void SysThreads::yield()
{
    CScheduler* ptr = CScheduler::Get();
    if (ptr) { ptr->Yield(); }
}

void SysThreads::delay(int milliseconds)
{
    CScheduler* ptr = CScheduler::Get();
    if (ptr) { ptr->MsSleep(milliseconds); }
}

}
