#ifndef __ShaderParameter_H__
#define __ShaderParameter_H__


#include <string>
#include "../fwd.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
#include "Support/Serialization/Serialization_Color.hpp"
using namespace GameLib1::Serialization;


class CTextureParam : public IArchiveObjectBase
{
public:

	CTextureResourceDesc m_Desc;

	CTextureHandle m_Handle;

public:

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Desc;
	}
};


template<typename T>
class CShaderParameter : public IArchiveObjectBase
{
	int m_ShaderManagerID;
	int m_ParameterIndex;

	std::string m_ParameterName;

//	std::vector<T> m_vecParameter;
	T m_Parameter;

public:

	CShaderParameter( const std::string& param_name = "" )
		:
	m_ParameterName(param_name),
	m_ShaderManagerID(-2),
	m_ParameterIndex(-1)
	{}

	T& Parameter() { return m_Parameter; }

	const std::string& GetParameterName() const { return m_ParameterName; }

	const T& GetParameter() const { return m_Parameter; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_ParameterName;

		ar & m_Parameter;

		if( ar.GetMode() == IArchive::MODE_INPUT )
		{
			m_ShaderManagerID = -1;
			m_ParameterIndex = -2;
		}
	}

	friend class CShaderManager;
};


class CShaderParameterGroup : public IArchiveObjectBase
{
public:

	std::vector< CShaderParameter< std::vector<int> > > m_Int;

	std::vector< CShaderParameter< std::vector<float> > > m_Float;

	/// array index is used as the texture stage
	std::vector< CShaderParameter<CTextureParam> > m_Texture;

public:

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Int & m_Float & m_Texture;
	}
};




#endif  /*  __ShaderParameter_H__  */
