#ifndef __indexed_vector_HPP__
#define __indexed_vector_HPP__


#include <vector>


namespace amorphous
{


template<typename T>
class indexed_vector
{
	std::vector<T> m_elements;

	unsigned int m_index;

public:

	indexed_vector() : m_index(0) {}

	bool get_current( T& dest )
	{
		if( m_elements.empty() )
			return false;

		if( (unsigned int)m_elements.size() <= m_index )
			return false;

		dest = m_elements[m_index];
		return true;
	}

	void next()
	{
		if( m_elements.empty() )
			return;

		if( m_index == (unsigned int)m_elements.size() -1 )
			m_index = 0;
		else
			m_index += 1;
	}

	void prev()
	{
		if( m_elements.empty() )
			return;

		// Of course, you can write it with a single line,
//		m_index = (m_index + m_elements.size() - 1) % m_elements.size();

		// but we just do it in a more intuitive way.
		if( m_index == 0 )
			m_index = (unsigned int)m_elements.size() - 1;
		else
			m_index -= 1;
	}

	void push_back( const T& val )
	{
		m_elements.push_back( val );
	}

	void resize( unsigned int size )
	{
		m_elements.resize( size );
	}

	bool empty() const
	{
		return m_elements.empty();
	}

	void clear()
	{
		m_elements.clear();
	}

	const T& operator[]( unsigned int index ) const
	{
		return m_elements[index];
	}

	T& operator[]( unsigned int index )
	{
		return m_elements[index];
	}

	void operator=( const std::vector<T>& src )
	{
		m_elements = src;
	}

	void operator++(int) { next(); }
	void operator--(int) { prev(); }
};


} // amorphous



#endif /* __indexed_vector_HPP__ */
