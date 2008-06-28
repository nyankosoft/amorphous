#ifndef  __AlphaEntity_H__
#define  __AlphaEntity_H__


#include "Stage/fwd.h"
#include "Stage/CopyEntity.h"
#include "Stage/EntityMotionPathRequest.h"
#include "fwd.h"


class CAlphaEntityDesc;


class CAlphaEntity : public CCopyEntity
{
	uint m_AlphaMaterialIndex;

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
};


#endif		/*  __AlphaEntity_H__  */
