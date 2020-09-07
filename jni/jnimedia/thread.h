
#ifndef _FFMPEGPLAYER_Thread_H_
#define _FFMPEGPLAYER_Thread_H_

#include <pthread.h>
#include <sys/types.h>

/*
 * Simple mutex class.  The implementation is system-dependent.
 *
 * The mutex must be unlocked by the thread that locked it.  They are not
 * recursive, i.e. the same thread can't lock it multiple times.
 */
class Mutex {
public:
    enum {
        PRIVATE = 0,
        SHARED = 1
    };

    Mutex();
    Mutex(const char* name);
    Mutex(int type, const char* name = NULL);
    ~Mutex();

    // lock or unlock the mutex
    int         lock();
    void        unlock();

    // lock if possible; returns 0 on success, error otherwise
    int         tryLock();

    // Manages the mutex automatically. It'll be locked when Autolock is
    // constructed and released when Autolock goes out of scope.
    class AutoLock {
    public:
        inline AutoLock(Mutex& mutex) : mLock(mutex)  { mLock.lock(); }
        inline AutoLock(Mutex* mutex) : mLock(*mutex) { mLock.lock(); }
        inline ~AutoLock() { mLock.unlock(); }
    private:
        Mutex& mLock;
    };

private:

    // A mutex cannot be copied
    Mutex(const Mutex&);
    Mutex&      operator = (const Mutex&);

    pthread_mutex_t mMutex;
};



class Sigmore {
public:
	Sigmore();
	~Sigmore();
	void lock();
	void unlock();
	//需要等待的线程调用该函数
	void wait();
	//随机唤醒某个正在等待的线程
	void wake();
	//唤醒所有的等待的线程
	void wakeAll();
private:
	pthread_cond_t mCond;
	pthread_mutex_t mLock;
};

class Thread
{
public:
    Thread();
    Thread(const char* name);
    virtual ~Thread();

    void                    start();
    int                     join();
    bool                    isRunning() { return mRunning; };

protected:
    bool                    mRunning;

    virtual void            run();

private:
    pthread_t               mThread;
    pthread_mutex_t         mLock;
    pthread_cond_t          mCondition;
    const char*             mName;

    void                    init();
    static void*            handleThreadStart(void* ptr);
};

#endif // Thread_H
