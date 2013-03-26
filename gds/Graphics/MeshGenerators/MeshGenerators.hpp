#ifndef __MeshGenerators_HPP__
#define __MeshGenerators_HPP__


#include "MeshGenerator.hpp"


namespace amorphous
{


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


class BoxMeshGenerator : public MeshGenerator
{
	Vector3 m_vEdgeLengths;

public:

	BoxMeshGenerator();

	~BoxMeshGenerator() {}

	void SetEdgeLengths( Vector3 vEdgeLengths ) { m_vEdgeLengths = vEdgeLengths; }

	Result::Name Generate();

	/// \param vLengths full edge lengths
	Result::Name Generate( Vector3 vLengths,
		U32 vertex_flags = DEFAULT_VERTEX_FLAGS,
		const SFloatRGBAColor& diffuse_color = SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f),
		MeshPolygonDirection::Type polygon_direction = MeshPolygonDirection::OUTWARD );
};


class CylinderMeshGenerator : public MeshGenerator
{
	CylinderDesc m_Desc;

public:

	CylinderMeshGenerator( const CylinderDesc& desc )
		:
	m_Desc(desc)
	{}

	~CylinderMeshGenerator() {}

	Result::Name Generate();
};


class ConeMeshGenerator : public MeshGenerator
{
	ConeDesc m_Desc;

public:

	ConeMeshGenerator( const ConeDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};


class SphereMeshGenerator : public MeshGenerator
{
	SphereDesc m_Desc;

public:

	SphereMeshGenerator( const SphereDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};


class CapsuleMeshGenerator : public MeshGenerator
{
	CCapsuleDesc m_Desc;

public:

	CapsuleMeshGenerator( const CCapsuleDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};

} // namespace amorphous



#endif  /* __MeshGenerators_HPP__ */
