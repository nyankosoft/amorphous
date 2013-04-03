#ifndef __linear_interpolation_HPP__
#define __linear_interpolation_HPP__


namespace amorphous
{

/// \brief Performes a uniform linear interpolation.
template<typename T>
inline T get_linearly_interpolated_value( const T *src, unsigned int num_elements, float position )
{
	if( num_elements == 0 )
		return T();

	if( num_elements == 1 )
		return src[0];

	if( position < 0 ) position = 0;
	else if( 1 < position ) position = 1;

	size_t index0 = (size_t)((float)(num_elements-1) * position);
	size_t index1 = index0 + 1;
	if( num_elements <= index1 )
		return src[index0];

	float inv = 1.0f / (float)(num_elements-1);
	float local_position = ( position - (inv * (float)index0) ) / inv;

	T value = src[index0] * (1.0f - local_position) + src[index1] * local_position;

	return value;
}

/// \brief Performes a uniform linear interpolation, i.e. a linear interpolation where intervals between the elements are the same.
/// \param[in] src
/// \param[in] position [0,1]
template<typename T>
inline T get_linearly_interpolated_value( const std::vector<T>& src, float position )
{
	if( src.empty() )
		return T();

	return get_linearly_interpolated_value( &src[0], (unsigned int)src.size(), position );
}


/// \brief Performes a non-uniform linear interpolation, i.e. a linear interpolation where intervals between the elements are different.
/// \param[in] src
/// \param[in] position [0,1]
template<typename T>
inline T get_linearly_interpolated_value( const std::vector< std::pair<float,T> >& table, float position )
{
	if( table.empty() )
		return T();

	const size_t num_elements = table.size();

	if( num_elements == 1 )
		return table[0].second;

	for( size_t i=1; i<num_elements; i++ )
	{
		if( position < table[i].first )
		{
			const float f = (position - table[i-1].first) / (table[i].first - table[i-1].first);
			return ( table[i-1].second * (1.0f - f) + table[i].second * f );
		}
	}

	/// \param position [in] position in the table [0.0,0.1]
	return table.back().second;
}



/// \brief Non-uniform linear interpolation.
/// Rationale for having a class: automates the process of adding a value to the correct position.
/// See set() function for more information.
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
		return get_linearly_interpolated_value( m_table, position );
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

	std::vector< std::pair<float,T> >& table() { return m_table; }
};

} // namespace amorphous


#endif /* __linear_interpolation_HPP__ */
