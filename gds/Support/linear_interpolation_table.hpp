#ifndef __linear_interpolation_table_HPP__
#define __linear_interpolation_table_HPP__


template<typename T>
class linear_interpolation_table
{
	std::vector< std::pair<float,T> > m_table;

public:

	linear_interpolation_table(){}

	linear_interpolation_table( const std::vector< std::pair<float,T> >& table )
	:
	m_table(table)
	{}

	~linear_interpolation_table(){}

	/// \param position [in] position in the table [0.0,0.1]
	T get( float position ) const
	{
		if( m_table.empty() )
			return T();

		const size_t num_elements = m_table.size();

		if( num_elements == 1 )
			return m_table[0].second;

		for( size_t i=1; i<num_elements; i++ )
		{
			if( position < m_table[i].first )
			{
				const float f = (position - m_table[i-1].first) / (m_table[i].first - m_table[i-1].first);
				return ( m_table[i-1].second * (1.0f - f) + m_table[i].second * f );
			}
		}

	/// \param position [in] position in the table [0.0,0.1]
		return m_table.back().second;
	}

	void set( float position, const T& obj )
	{
		if( m_table.empty() )
		{
			m_table.push_back( std::pair<float,T>(position,obj) );
			return;
		}

		const size_t num_elements = m_table.size();
		for( size_t i=0; i<num_elements; i++ )
		{
			if( position <= m_table[i].first )
			{
				m_table.insert( m_table.begin() + i, std::pair<float,T>(position,obj) );
				return;
			}
		}

		m_table.push_back( std::pair<float,T>(position,obj) );
	}
};



#endif /* __linear_interpolation_table_HPP__ */
