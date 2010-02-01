#ifndef  __fixed_string_HPP__
#define  __fixed_string_HPP__


#include <stdlib.h>
#include <string.h>

#include <string>


//namespace __framework_name__
//{


template<int MaxLength>
class fixed_string
{
	char m_buffer[MaxLength];

public:

	fixed_string()
	{
		memset( m_buffer, 0, sizeof(m_buffer) );
	}

	fixed_string( const char *src )
	{
		memset( m_buffer, 0, sizeof(m_buffer) );
		if( src )
			strncpy( m_buffer, src, MaxLength - 1 );
	}

	template<int RHSMaxLength>
	fixed_string( const fixed_string<RHSMaxLength>& rhs )
	{
		memset( m_buffer, 0, sizeof(m_buffer) );
		strncpy( m_buffer, rhs.c_str(), MaxLength - 1 );
	}

	~fixed_string()
	{
	}

	const char *c_str() const
	{
		return m_buffer;
	}

	size_t length() const
	{
		return strlen(m_buffer);
	}

	template<int RetMaxLength>
	fixed_string<RetMaxLength> substr( int pos, int len = -1 )
	{
		fixed_string<RetMaxLength> ret;
	}

	template<int RHSMaxLength>
	const fixed_string<MaxLength>& operator=( const fixed_string<RHSMaxLength>& rhs )
	{
		strncpy( m_buffer, rhs.c_str(), MaxLength - 1 );
		return *this;
	}

	template<int RHSMaxLength>
	const fixed_string<MaxLength+RHSMaxLength> operator+( const fixed_string<RHSMaxLength>& rhs ) const
	{
		fixed_string<MaxLength+RHSMaxLength> out(*this);
		out += rhs;
		return out;
	}

	template<int RetMaxLength>
	const fixed_string<MaxLength>& operator+=( const fixed_string<RetMaxLength>& rhs )
	{
		size_t rest = MaxLength - this->length();
		strncat( m_buffer, rhs.c_str(), rest - 1 );
		return *this;
	}

	//
	// Support for std::string
	//

	fixed_string( const std::string& src )
	{
		memset( m_buffer, 0, sizeof(m_buffer) );
		strncpy( m_buffer, src.c_str(), MaxLength - 1 );
	}

	std::string string() const
	{
		return std::string( this->c_str() );
	}

/*	std::string substr( int pos, int len = -1 )
	{
	}

	size_t find( const std::string& )
	{
	}*/

	fixed_string<MaxLength> operator=( const std::string& rhs )
	{
		strncpy( m_buffer, rhs.c_str(), MaxLength - 1 );
		return *this;
	}

	fixed_string<MaxLength> operator=( const char *rhs )
	{
		strncpy( m_buffer, rhs, MaxLength - 1 );
		return *this;
	}
};

/*
template<int LHSMaxLength, int RHSMaxLength>
const fixed_string<MaxLength> operator+( const fixed_string<LHSMaxLength>& lhs, const fixed_string<RHSMaxLength>& rhs )
{
	fixed_string<MaxLength> out(*this);
	out += rhs;
	return out;
}
*/

//} // namespace __framework_name__



#endif /* __fixed_string_HPP__ */
