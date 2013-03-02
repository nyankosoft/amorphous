#ifndef __stl_aux_HPP__
#define __stl_aux_HPP__


#include <vector>


namespace amorphous
{


template<typename T>
inline void group_elements( const std::vector<T>& src, std::vector< std::pair< T, std::vector<unsigned int> > >& element_and_indices_pairs )
{
	using std::vector;
	using std::pair;

	const size_t num_elements = src.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		size_t j = 0;
		for( j=0; j<element_and_indices_pairs.size(); j++ )
		{
			if( src[i] == element_and_indices_pairs[j].first )
			{
				element_and_indices_pairs[j].second.push_back( (unsigned int)i );
				break;
			}
		}

		if( j == element_and_indices_pairs.size() )
		{
			element_and_indices_pairs.push_back( pair< T, vector<unsigned int> >() );
			element_and_indices_pairs.back().first = src[i];
			element_and_indices_pairs.back().second.push_back( (unsigned int)i );
		}
	}
}

} // amorphous



#endif /* __stl_aux_HPP__ */
