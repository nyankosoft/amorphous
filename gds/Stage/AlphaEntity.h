#ifndef  __AlphaEntity_H__
#define  __AlphaEntity_H__


#include "Stage/fwd.h"
#include "Stage/CopyEntity.h"
#include "Stage/EntityMotionPathRequest.h"
#include "fwd.h"


class CAlphaEntityDesc;


class CAlphaEntity : public CCopyEntity
{
	int m_AlphaMaterialIndex;

	// Note this vector always stores single element
	// reason: need to set as an argument of CBaseEntity::DrawMeshObject()
//	std::vector<int> m_SingleAlphaMaterialIndex;

	/// taken from the shader technique table of the owner entity
	/// - usually the highest resolution technique of the target subset
	CShaderTechniqueHandle m_ShaderTechnique;

public:

	CAlphaEntity();

	virtual ~CAlphaEntity();

//	virtual void Init( CCopyEntityDesc& desc );

//	void Init( CAlphaEntityDesc& desc );

	virtual void Update( float dt );

//	virtual void UpdatePhysics( float dt );

	void Draw();

	virtual void HandleMessage( SGameMessage& msg );

	virtual void TerminateDerived();

	void SetShaderTechnique( CShaderTechniqueHandle& tech ) { m_ShaderTechnique = tech; }

	void SetShaderTechniqueName( const std::string& shared_technique_name ) { m_ShaderTechnique.SetTechniqueName( shared_technique_name.c_str() ); }

	void SetAlphaMaterialIndex( int alpha_mat_index ) { m_AlphaMaterialIndex = alpha_mat_index; }
};


#endif		/*  __AlphaEntity_H__  */
