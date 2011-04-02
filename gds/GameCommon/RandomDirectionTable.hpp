#ifndef  __RANDOMDIRECTIONTABLE_H__
#define  __RANDOMDIRECTIONTABLE_H__


#include "../3DMath/Vector3.hpp"


#define  RANDDIRTBL_NUM_RANDOM_DIRECTIONS	(32*32)

#define RANDOM_DIRECTION_TABLE ( CRandomDirectionTable::ms_SingletonInstance_ )

class CRandomDirectionTable
{
private:
	Vector3 m_avRandomDirectionTable[RANDDIRTBL_NUM_RANDOM_DIRECTIONS];

protected:
	CRandomDirectionTable();		//singleton

public:
	static CRandomDirectionTable ms_SingletonInstance_;	//single instance of 'CRandomDirectionTable'

//	~CRandomDirectionTable();
	inline Vector3 GetRandomDirection();

};



// ================================ inline implementations ================================ 

inline Vector3 CRandomDirectionTable::GetRandomDirection()
{
	int iRandNum = rand() * RANDDIRTBL_NUM_RANDOM_DIRECTIONS / RAND_MAX;

	if( RANDDIRTBL_NUM_RANDOM_DIRECTIONS <= iRandNum )
		iRandNum = RANDDIRTBL_NUM_RANDOM_DIRECTIONS -1;

	return m_avRandomDirectionTable[ iRandNum ];
}



#endif		/*  __RANDOMDIRECTIONTABLE_H__  */
