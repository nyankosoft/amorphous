
#include "BinaryDatabase.hpp"
using namespace amorphous::serialization;

#include "amorphous/Support/memory_helpers.hpp"

/*
template<class KeyType>
void CBinaryDatabase::HandleData( void *pData, const int size )
{
	switch( GetMode() )
	{
	case MODE_OUTPUT:
		fwrite( pData, size, 1, m_pFile );
		break;

	case MODE_INPUT:
		fread( pData, size, 1, m_pFile );
		break;

	default:
		assert( !"CBinaryDatabase::HandleData() - an invalid mode detected" );
		break;
	}
}
*/
