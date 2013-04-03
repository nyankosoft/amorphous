namespace amorphous
{

namespace physics
{

class CRay;
class CRaycastHit;

class CStream;

class CScene;
class CSceneDesc;
class CSceneLimits;
class CActor;
class CActorDesc;

// shapes
class CShape;
class CBoxShape;
class CSphereShape;
class CCapsuleShape;
class CTriangleMeshShape;

// shape descs for each shape (+ raw version for triangle mesh)
class CShapeDesc;
class CBoxShapeDesc;
class CSphereShapeDesc;
class CCapsuleShapeDesc;
class CConvexShapeDesc;
class CTriangleMeshShapeDesc;
class CRawTriangleMeshShapeDesc;

class CJoint;
class CJointImpl;
class CJointDesc;
class CFixedJointDesc;
class CSphericalJointDesc;
class CRevoluteJointDesc;
// derived joint classes
class CMaterial;
class CMaterialDesc;

class CCloth;
class CClothDesc;
class CClothMesh;
class CClothMeshDesc;

class CConvexMesh;
class CConvexMeshDesc;

class CTriangleMesh;
class CTriangleMeshDesc;

class CPreprocessor;
class CPreprocessorImpl;

class CContactStreamIterator;
class CContactPair;
class CUserContactReport;


} // namespace physics

} // namespace amorphous
