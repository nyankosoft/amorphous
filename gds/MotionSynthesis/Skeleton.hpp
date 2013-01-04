#ifndef  __Skeleton_H__
#define  __Skeleton_H__


#include "gds/Support/Serialization/Serialization.hpp"
#include "gds/Support/Serialization/Serialization_3DMath.hpp"
#include "fwd.hpp"
#include "TransformNode.hpp"


namespace amorphous
{

using namespace serialization;


/// set a blend weight for a particular bone
//void SetBlendWeight( bone_name, weight );


namespace msynth
{


class CBone : public IArchiveObjectBase
{
	std::string m_Name;

	Vector3 m_vOffset;

	/// Orientation of the bone
	/// - Bones taken from LightWave scene file use this to store rest direction of bone
	Matrix33 m_matOrient;

	std::vector<CBone> m_vecChild;

public:

	CBone() : m_vOffset(Vector3(0,0,0)), m_matOrient(Matrix33Identity()) {}

	const std::string& GetName() const { return m_Name; }

	void SetName( const std::string& name ) { m_Name = name; }

	Vector3 GetOffset() const { return m_vOffset; }

	void SetOffset( const Vector3& offset ) { m_vOffset = offset; }

	Matrix33 GetOrient() const { return m_matOrient; }

	void SetOrient( const Matrix33& orientation ) { m_matOrient = orientation; }

	int GetNumChildren() const { return (int)m_vecChild.size(); }

	const CBone& GetChild( int index ) const { return m_vecChild[index]; }

	CBone& Child( int index ) { return m_vecChild[index]; }

	std::vector<CBone>& Children() { return m_vecChild; }

	void AddChildBone( CBone& bone ) { m_vecChild.push_back( bone ); }

	void Scale_r( float factor );

	/*inline*/ void CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const CTransformNode& input_node ) const;

	bool CreateLocator( const std::string& bone_name, std::vector<int>& locator ) const;

	void CreateEmptyTransformNodeTree( CTransformNode& parent_transform_node );

	Vector3 CalculateNodePositionInSkeletonSpace(
		const std::vector<int>& node_locator,
		uint& index,
		const Transform& parent_transform,
		const CTransformNode& parent_transform_node
		) const;

	void DumpToTextFile( FILE* fp, int depth );

	void Serialize( IArchive & ar, const unsigned int version );

	virtual unsigned int GetVersion() const { return 1; }
};

/*
inline void CBone::CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const CTransformNode& input_node ) const
{
	...
}
*/


class CSkeleton : public IArchiveObjectBase
{
	CBone m_RootBone;

	static const CSkeleton m_Null;

public:

	const CBone& GetRootBone() const { return m_RootBone; }

	CBone& RootBone() { return m_RootBone; }

	void SetBones( const CBone& root_bone ) { m_RootBone = root_bone; }

	void Scale( float scaling_factor ) { m_RootBone.Scale_r( scaling_factor ); }

	bool CreateLocator( const std::string& bone_name, std::vector<int>& locator ) const;

	void CreateEmptyTransformNodeTree( CTransformNode& root_transform_node ) { m_RootBone.CreateEmptyTransformNodeTree( root_transform_node ); }

	Vector3 CalculateNodePositionInSkeletonSpace( const std::vector<int> node_locator, const CKeyframe& keyframe ) const;

	void DumpToTextFile( const std::string& output_filepath );

	void Serialize( IArchive & ar, const unsigned int version ) { ar & m_RootBone; }

	static const CSkeleton& GetNull() { return m_Null; }
};


/// created per target skeletal mesh ?
class CWeightBlendSkeleton
{
public:
};

class CWeightBlendBone
{
public:
	Vector3 m_vOffset;
	float m_afBlendWeight[8];
};


} // namespace msynth

} // namespace amorphous



#endif /* __Skeleton_H__ */
