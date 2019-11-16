//
// Created by matthew on 1/11/19.
//

#ifndef PROJECTM_BELLEW_BACKGROUNDWORKER_H
#define PROJECTM_BELLEW_BACKGROUNDWORKER_H



// Small class to encapsulate synchronization of a simple background task runner
// see projectM.cpp

class BackgroundWorkerSync
{
    pthread_mutex_t mutex;
    pthread_cond_t  condition_start_work;
    pthread_cond_t  condition_work_done;
    volatile bool there_is_work_to_do;
    volatile bool finished;

public:
    BackgroundWorkerSync() : there_is_work_to_do(false), finished(false)
    {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&condition_start_work, NULL);
        pthread_cond_init(&condition_work_done, NULL);
    }

    void reset()
    {
        there_is_work_to_do = false;
        finished = false;
    }

    // called by foreground
    void wake_up_bg()
    {
        pthread_mutex_lock(&mutex);
        there_is_work_to_do = true;
        pthread_cond_signal(&condition_start_work);
        pthread_mutex_unlock(&mutex);
    }

    // called by foreground
    void wait_for_bg_to_finish()
    {
        pthread_mutex_lock(&mutex);
        while (there_is_work_to_do)
            pthread_cond_wait(&condition_work_done, &mutex);
        pthread_mutex_unlock(&mutex);
    }

    // called by foreground() when shutting down, background thread should exit
    void finish_up()
    {
        pthread_mutex_lock(&mutex);
        finished = true;
        pthread_cond_signal(&condition_start_work);
        pthread_mutex_unlock(&mutex);
    }

    // called by background
    bool wait_for_work()
    {
        pthread_mutex_lock(&mutex);
        while (!there_is_work_to_do && !finished)
            pthread_cond_wait(&condition_start_work, &mutex);
        pthread_mutex_unlock(&mutex);
        return !finished;
    }

    // called by background
    void finished_work()
    {
        pthread_mutex_lock(&mutex);
        there_is_work_to_do = false;
        pthread_cond_signal(&condition_work_done);
        pthread_mutex_unlock(&mutex);
    }

};


#endif //PROJECTM_BELLEW_BACKGROUNDWORKER_H
