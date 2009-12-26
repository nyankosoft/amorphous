#ifndef __time_HPP__
#define __time_HPP__


#ifdef _MSC_VER
#include "Winsock2.h"
#else
#include <sys/time.h>
#endif /* _MSC_VER */


class stime
{
	struct timeval m_timeval;

public:

	stime()
	{
		m_timeval.tv_sec  = 0;
		m_timeval.tv_usec = 0;
	}

	stime( const struct timeval& t )
		:
	m_timeval(t)
	{}

	stime operator-( const stime& rhs ) const
	{
		struct timeval out;
		if( rhs.m_timeval.tv_usec <= m_timeval.tv_usec )
		{
			out.tv_sec  = m_timeval.tv_sec  - rhs.m_timeval.tv_usec;
			out.tv_usec = m_timeval.tv_usec - rhs.m_timeval.tv_usec;
		}
		else
		{
			out.tv_sec  = m_timeval.tv_sec  - rhs.m_timeval.tv_usec - 1;
			out.tv_usec = m_timeval.tv_usec + 1000000 - rhs.m_timeval.tv_usec;
		}

		return stime(out);
	}

	double get_sec() const
	{
		return (double)m_timeval.tv_sec + (double)m_timeval.tv_usec / 1000000.0;
	}

	int get_millisec() const
	{
		return (int)m_timeval.tv_sec * 1000 + (int)m_timeval.tv_usec / 1000;
	}
};



#endif /* __time_HPP__ */
