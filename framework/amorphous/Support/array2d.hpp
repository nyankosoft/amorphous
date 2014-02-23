#ifndef  __array2d_HPP__
#define  __array2d_HPP__


#include <vector>


namespace amorphous
{


template<class T>
class array2d
{
	/// holds values of 2d array in a 1d array
	/// - size of this vector must always be nx * ny
	std::vector<T> m_vecData;

	int nx, ny;

public:

	inline array2d() { nx = ny = 0; }

	~array2d() {}

	inline void resize( int x, int y );

	inline void resize( int x, int y, const T& val );

	inline void clear();

	inline int size_x() const { return nx; }
	inline int size_y() const { return ny; }

	/// grow the 2d array in row(y) direction by 'amount_y'
	inline void increase_y( int amount_y, const T& val = T() );

	/// grow the 2d array in column(x) direction by 'amount_x'
	inline void increase_x( int amount_x, const T& val = T() );

	inline T& operator()( int x, int y );

	inline const T& operator()( int x, int y ) const;

};


//=============================== inline implementations ===============================


template<class T>
inline void array2d<T>::resize( int size_x, int size_y )
{
	nx = size_x;
	ny = size_y;
	m_vecData.resize( size_x * size_y );
}


template<class T>
inline void array2d<T>::resize( int size_x, int size_y, const T& val )
{
	nx = size_x;
	ny = size_y;
	m_vecData.resize( size_x * size_y );
	int i;
	for( i=0; i<size_x*size_y; i++ )
		m_vecData[i] = val;
}


template<class T>
inline void array2d<T>::increase_y( int amount_y, const T& val )
{
	int i;
	for( i=0; i<amount_y*nx; i++ )
		m_vecData.push_back( val );

	ny += amount_y;
}


template<class T>
inline void array2d<T>::increase_x( int amount_x, const T& val )
{
	int i, j;
	typename std::vector<T>::iterator itr;
	for( i=0; i<amount_x; i++ )
	{
		for( j=0; j<ny; j++ )
		{
			itr = m_vecData.begin() + (nx+i)*j;
			m_vecData.insert( itr, val );
		}
	}

	nx += amount_x;
}


/*
template<class T>
inline void array2d<T>::resize( int size_x, int size_y, T val )
{
	nx = size_x;
	ny = size_y;
	m_vecData.resize( size_x * size_y );
	int i;
	for( i=0; i<size_x*size_y; i++ )
		m_vecData[i] = val;
}
*/

template<class T>
inline T& array2d<T>::operator()( int x, int y )
{
	return m_vecData.at( y * nx + x );
}


template<class T>
inline const T& array2d<T>::operator()( int x, int y ) const
{
	return m_vecData.at( y * nx + x );
}


template<class T>
inline void array2d<T>::clear()
{
	return m_vecData.clear();
}

} // amorphous



#endif		/*  __array2d_HPP__  */
