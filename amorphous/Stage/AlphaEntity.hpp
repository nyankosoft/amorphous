#ifndef  __AlphaEntity_H__
#define  __AlphaEntity_H__


#include "Stage/fwd.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/EntityMotionPathRequest.hpp"
#include "Support/shared_prealloc_pool.hpp"


namespace amorphous
{


class AlphaEntityDesc;


class AlphaEntity : public CCopyEntity
{
//	shared_prealloc_pool<AlphaEntity> *m_pPool;

	int m_AlphaMaterialIndex;

	// Note this vector always stores single element
	// reason: need to set as an argument of BaseEntity::DrawMeshObject()
//	std::vector<int> m_SingleAlphaMaterialIndex;

	/// taken from the shader technique table of the owner entity
	/// - usually the highest resolution technique of the target subset
	ShaderTechniqueHandle m_ShaderTechnique;

public:

	AlphaEntity();

	virtual ~AlphaEntity();

//	virtual void Init( CCopyEntityDesc& desc );

//	void Init( AlphaEntityDesc& desc );

	virtual void Update( float dt );

//	virtual void UpdatePhysics( float dt );

	void Draw();

	virtual void HandleMessage( GameMessage& msg );

	virtual void TerminateDerived();

	void SetShaderTechnique( ShaderTechniqueHandle& tech ) { m_ShaderTechnique = tech; }

	void SetShaderTechniqueName( const std::string& shared_technique_name ) { m_ShaderTechnique.SetTechniqueName( shared_technique_name.c_str() ); }

	void SetAlphaMaterialIndex( int alpha_mat_index ) { m_AlphaMaterialIndex = alpha_mat_index; }
};

} // namespace amorphous



#endif		/*  __AlphaEntity_H__  */
