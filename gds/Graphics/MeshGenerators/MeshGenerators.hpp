#ifndef __MeshGenerators_HPP__
#define __MeshGenerators_HPP__


#include "MeshGenerator.hpp"


class TexCoordStyle
{
public:
	enum Flags
	{
//		LINEAR_SHIFT_X     = (1 << 0),
//		LINEAR_SHIFT_INV_X = (1 << 1),
		LINEAR_SHIFT_Y     = (1 << 2),
		LINEAR_SHIFT_INV_Y = (1 << 3),
//		LINEAR_SHIFT_Z     = (1 << 4),
//		LINEAR_SHIFT_INV_Z = (1 << 5),
	};
};


class CBoxMeshGenerator : public CMeshGenerator
{
	Vector3 m_vEdgeLengths;

public:

	CBoxMeshGenerator();

	~CBoxMeshGenerator() {}

	void SetEdgeLengths( Vector3 vEdgeLengths ) { m_vEdgeLengths = vEdgeLengths; }

	Result::Name Generate();

	/// \param vLengths full edge lengths
	Result::Name Generate( Vector3 vLengths,
		U32 vertex_flags = DEFAULT_VERTEX_FLAGS,
		const SFloatRGBAColor& diffuse_color = SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f),
		MeshPolygonDirection::Type polygon_direction = MeshPolygonDirection::OUTWARD );
};


class CCylinderMeshGenerator : public CMeshGenerator
{
	CCylinderDesc m_Desc;

public:

	CCylinderMeshGenerator( const CCylinderDesc& desc )
		:
	m_Desc(desc)
	{}

	~CCylinderMeshGenerator() {}

	Result::Name Generate();
};


class CConeMeshGenerator : public CMeshGenerator
{
	CConeDesc m_Desc;

public:

	CConeMeshGenerator( const CConeDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};


class CSphereMeshGenerator : public CMeshGenerator
{
	CSphereDesc m_Desc;

public:

	CSphereMeshGenerator( const CSphereDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};


class CCapsuleMeshGenerator : public CMeshGenerator
{
	CCapsuleDesc m_Desc;

public:

	CCapsuleMeshGenerator( const CCapsuleDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};


#endif  /* __MeshGenerators_HPP__ */
