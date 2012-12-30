
#ifndef  __PREALLOCDYNAMICLINKLIST_SERIALIZER_H__
#define  __PREALLOCDYNAMICLINKLIST_SERIALIZER_H__

#include "Support/PreAllocDynamicLinkList.hpp"
#include "Support/Serialization/ArchiveObjectBase.hpp"

using namespace amorphous;

template<class T>
class TCPADLinkListSerializer : public IArchiveObjectBase
{

	TCPreAllocDynamicLinkList<T> *m_pLinkList;

public:

	TCPADLinkListSerializer( TCPreAllocDynamicLinkList<T>& linklist )
	{
		m_pLinkList = &linklist;
	}

	~TCPADLinkListSerializer() {}

//	void MemberFunction01();

	void Serialize( IArchive& ar, const unsigned int version )
	{
		TCPreAllocDynamicLinkList<T>::LinkListIterator itr;

		for( itr = m_pLinkList->Begin(); itr != m_pLinkList->End(); itr++ )
		{
			ar & (*itr);
		}
	}

};



#endif		/*  __PREALLOCDYNAMICLINKLIST_SERIALIZER_H__  */


//----------------------------------- cut here -----------------------------------

/*
#include "PADLinkListSerializer.hpp"

TCPADLinkListSerializer::TCPADLinkListSerializer( TCPreAllocDynamicLinkList<T>& linklist )
{
	m_pLinkList = &linklist;
}

TCPADLinkListSerializer::~TCPADLinkListSerializer()
{
}


void CSampleClass::MemberFunction01()
{
}

void CSampleClass::Serialize( IArchive& ar, const unsigned int version )
{
	TCPreAllocDynamicLinkList<T>::LinkListIterator itr;

	for( itr = m_pLinkList->Begin(); itr != m_pLinkList->End(); itr++ )
	{
		ar & itr;
	}
}
*/