#ifndef __GDS_SINGLETON_H__
#define __GDS_SINGLETON_H__


#include <boost/thread/mutex.hpp>


namespace amorphous
{


/**
  Taken from YaneSDK 3rd
  - singleton	: singleton template
  - programmed & desinged by yaneurao(M.Isozaki) '02/03/13

  Holds a pointer that creates its instance only if it is needed.
  You can use this class as a non-local static object.

  class CHoge{
  public:
  	static singleton<CHoge> m_obj;
  	static CHoge* GetObj() { return m_obj->get(); }
  };

  - The object is automatically released.
  - Only one instance of this class can be created.
  - Can only be used as a static object.
*/
template <class T>
class singleton
{

public:

	/// Do nothing in ctor.
	/// Nothing should be done for some reason related to the initialization
	/// of non-local static objects.
	singleton()
	{
	}

	~singleton() { Release(); }

	///	mechanism to make the singleton look like a pointer
	T& operator*() { return *get(); }
	T* operator->() { return get();	}

	T* get() { CheckInstance(); return m_lpObj; }

	///	returns true if the object is NULL
	bool	isNull() const { return m_lpObj == NULL; }

	///	Force the singleton class to release the instance.
	/// Also done in dtor.
	void Release();

	/**
		Check if the singleton instance has been created.
		This is useful if the instantiation takes time and you want to
		initialize it in advance.
		If not, you don't have to call this function.
	*/
	void CheckInstance();


protected:

	static boost::mutex m_Mutex;

	/// define as static to guarantee that the pointer is NULL.
	/// during the link phase, which comes before the initialization of non-local static objects.
	static T*	m_lpObj;
};

///////////////////////////////////////////////////////////////////////////////////////

// static objects
template <class T> T* singleton<T>::m_lpObj = 0;

template <class T> boost::mutex singleton<T>::m_Mutex;


template <class T> void	singleton<T>::CheckInstance()
{
	if (m_lpObj==NULL)
	{
		// Do double-checked locking
		boost::mutex::scoped_lock scoped_lock(m_Mutex);
		if (m_lpObj == NULL)
		{
			m_lpObj = new T;
		}
	}
}


template <class T> void	singleton<T>::Release()
{
	if (m_lpObj!=NULL)
	{
		// Do double-checked locking
		boost::mutex::scoped_lock scoped_lock(m_Mutex);
		if (m_lpObj != NULL)
		{
			delete m_lpObj;
			m_lpObj = NULL;
		}
	}
}


} // namespace amorphous


#endif  /*  __GDS_SINGLETON_H__  */
