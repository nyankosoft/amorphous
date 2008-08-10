#ifndef  __thread_starter_H__
#define  __thread_starter_H__


#include <boost/thread.hpp>


/**

*/
template <class TargetClass>
class thread_starter
{
protected:

	TargetClass *m_pTarget;

public:

	thread_starter( TargetClass *pTarget )
		:
	m_pTarget(pTarget)
	{
	}

	~thread_starter()
	{
	}

	void operator()()
	{
		m_pTarget->ThreadMain();
	}
};


template<class T>
inline boost::shared_ptr<boost::thread>start_thread( T *pTarget )
{
	thread_starter<T> starter(pTarget);
	boost::shared_ptr<boost::thread> pThread
		= boost::shared_ptr<boost::thread>( new boost::thread(starter) );

	return pThread;
}


/**
 Usage:
 class YourThreadClass : public thread_class
 {
	public:

	run()
	{
		// thread main loop
	}
 };

 YourThreadClass thread_object;
 thread_object.start_thread();


*/
class thread_class
{
private:

	boost::shared_ptr<boost::thread> m_pThread;

public:

	thread_class() {}

	virtual ~thread_class()
	{
		m_pThread.reset();
	}

	void join() { m_pThread->join(); }

	virtual void run() = 0;

	void start_thread()
	{
		thread_starter<thread_class> starter(this);
		m_pThread = boost::shared_ptr<boost::thread>( new boost::thread(starter) );
	}

	void ThreadMain()
	{
		run();
	}
};


#endif		/*  __thread_starter_H__  */
