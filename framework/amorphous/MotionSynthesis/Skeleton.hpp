#ifndef  __Skeleton_H__
#define  __Skeleton_H__


#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/Serialization_3DMath.hpp"
#include "fwd.hpp"
#include "TransformNode.hpp"


namespace amorphous
{

using namespace serialization;


/// set a blend weight for a particular bone
//void SetBlendWeight( bone_name, weight );


namespace msynth
{


class Bone : public IArchiveObjectBase
{
	std::string m_Name;

	Vector3 m_vOffset;

	/// Orientation of the bone
	/// - Bones taken from LightWave scene file use this to store rest direction of bone
	Matrix33 m_matOrient;

	std::vector<Bone> m_vecChild;

public:

	Bone() : m_vOffset(Vector3(0,0,0)), m_matOrient(Matrix33Identity()) {}

	const std::string& GetName() const { return m_Name; }

	void SetName( const std::string& name ) { m_Name = name; }

	Vector3 GetOffset() const { return m_vOffset; }

	void SetOffset( const Vector3& offset ) { m_vOffset = offset; }

	Matrix33 GetOrient() const { return m_matOrient; }

	void SetOrient( const Matrix33& orientation ) { m_matOrient = orientation; }

	int GetNumChildren() const { return (int)m_vecChild.size(); }

	const Bone& GetChild( int index ) const { return m_vecChild[index]; }

	Bone& Child( int index ) { return m_vecChild[index]; }

	std::vector<Bone>& Children() { return m_vecChild; }

	void AddChildBone( Bone& bone ) { m_vecChild.push_back( bone ); }

	void Scale_r( float factor );

	/*inline*/ void CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const TransformNode& input_node ) const;

	bool CreateLocator( const std::string& bone_name, std::vector<int>& locator ) const;

	void CreateEmptyTransformNodeTree( TransformNode& parent_transform_node );

	Vector3 CalculateNodePositionInSkeletonSpace(
		const std::vector<int>& node_locator,
		uint& index,
		const Transform& parent_transform,
		const TransformNode& parent_transform_node
		) const;

	void DumpToTextFile( FILE* fp, int depth );

	void Serialize( IArchive & ar, const unsigned int version );

	virtual unsigned int GetVersion() const { return 1; }

	static int get_htrans_rev();
};

/*
inline void Bone::CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const TransformNode& input_node ) const
{
	...
}
*/


class Skeleton : public IArchiveObjectBase
{
	Bone m_RootBone;

	static const Skeleton m_Null;

public:

	const Bone& GetRootBone() const { return m_RootBone; }

	Bone& RootBone() { return m_RootBone; }

	void SetBones( const Bone& root_bone ) { m_RootBone = root_bone; }

	void Scale( float scaling_factor ) { m_RootBone.Scale_r( scaling_factor ); }

	bool CreateLocator( const std::string& bone_name, std::vector<int>& locator ) const;

	void CreateEmptyTransformNodeTree( TransformNode& root_transform_node ) { m_RootBone.CreateEmptyTransformNodeTree( root_transform_node ); }

	Vector3 CalculateNodePositionInSkeletonSpace( const std::vector<int> node_locator, const Keyframe& keyframe ) const;

	void DumpToTextFile( const std::string& output_filepath );

	void Serialize( IArchive & ar, const unsigned int version ) { ar & m_RootBone; }

	static const Skeleton& GetNull() { return m_Null; }
};


/// created per target skeletal mesh ?
class WeightBlendSkeleton
{
public:
};

class WeightBlendBone
{
public:
	Vector3 m_vOffset;
	float m_afBlendWeight[8];
};


} // namespace msynth

} // namespace amorphous



#endif /* __Skeleton_H__ */
