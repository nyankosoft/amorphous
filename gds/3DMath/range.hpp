#ifndef __range_HPP__
#define __range_HPP__


#undef min
#undef max


namespace amorphous
{


template<typename T>
class range
{
public:

	T min, max;

	range() : min(T()),max(T()) {}

	range( const T& _min, const T& _max ) : min(_min),max(_max) {}

	bool is_valid_range() const { return (min < max); }

	void set( const T& _min, const T& _max ) { min = _min; max = _max; }
};


} // namespace amorphous


#endif /* __range_HPP__ */
