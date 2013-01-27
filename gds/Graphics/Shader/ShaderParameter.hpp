#ifndef __ShaderParameter_H__
#define __ShaderParameter_H__


#include <string>
#include "../fwd.hpp"
#include "../TextureHandle.hpp"
#include "../../Support/Serialization/Serialization.hpp"
#include "../../Support/Serialization/Serialization_3DMath.hpp"
#include "../../Support/Serialization/Serialization_Color.hpp"


namespace amorphous
{
using namespace serialization;


class CTextureParam : public IArchiveObjectBase
{
public:

	TextureResourceDesc m_Desc;

	TextureHandle m_Handle;

	int m_Stage;

public:

	CTextureParam()
		:
	m_Stage(-1)
	{}

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
	T m_Parameter; ///< This could be CTextureParam, int, float, Vector3, etc...

public:

	CShaderParameter( const std::string& param_name = "" )
		:
	m_ParameterName(param_name),
	m_ShaderManagerID(-2),
	m_ParameterIndex(-1)
	{}

	T& Parameter() { return m_Parameter; }

	const std::string& GetParameterName() const { return m_ParameterName; }

	void SetParameterName( const std::string& parameter_name ) { m_ParameterName = parameter_name; }

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

	friend class ShaderManager;
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


// default behavior of LoadShaderParam - do nothing.
template<typename T>
inline bool LoadShaderParam( CShaderParameter<T>& param )
{
	return true;
}


inline bool LoadShaderParam( CShaderParameter<CTextureParam>& param )
{
	return param.Parameter().m_Handle.Load( param.Parameter().m_Desc );
}



} // namespace amorphous



#endif  /*  __ShaderParameter_H__  */
