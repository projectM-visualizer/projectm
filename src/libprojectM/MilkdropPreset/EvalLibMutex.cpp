#include <projectm-eval.h>

#include <mutex>

static std::mutex s_evalMutex;

void projectm_eval_memory_host_lock_mutex()
{
    s_evalMutex.lock();
}

void projectm_eval_memory_host_unlock_mutex()
{
    s_evalMutex.unlock();
}
