#include <gds/3DMath/Matrix34.hpp>
#include <gds/3DMath/Matrix23.hpp>
#include <gds/3DMath/AABB3.hpp>
#include <gds/3DMath/AABB2.hpp>
#include <gds/Support/Vec3_StringAux.hpp>
#include <gds/Support/Log/DefaultLog.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>


static Matrix33 Matrix33IdentityForBoostPython()
{
	return Matrix33Identity();
}


BOOST_PYTHON_MODULE(math3d)
{
	using namespace boost;
	using namespace boost::python;

	class_<Vector3>("Vector3")
		.def(init<float,float,float>())
		.def_readwrite("x",&Vector3::x)
		.def_readwrite("y",&Vector3::y)
		.def_readwrite("z",&Vector3::z)
		.def(self + self)
		.def(self - self)
		.def(self += self)
		.def(self -= self)
		.def(self * float())
		.def(self / float())
	;

//	def( "Vec3Length", Vec3Length );
//	def( "Vec3GetNormalized", Vec3GetNormalized );

	class_<Vector2>("Vector2")
		.def(init<float,float>())
		.def_readwrite("x",&Vector2::x)
		.def_readwrite("y",&Vector2::y)
		.def(self + self)
		.def(self - self)
		.def(self += self)
		.def(self -= self)
		.def(self * float())
		.def(self / float())
	;

	class_<Matrix33>("Matrix33")
		.def(self + self)
		.def(self - self)
		.def(self * self)
		.def(self * Vector3())
//		.def("Orthonormalize",       &Matrix33::Orthonormalize )
		.def("SetIdentity",          &Matrix33::SetIdentity )
		.def("GetColumn",            &Matrix33::GetColumn )
		.def("SetColumn",            &Matrix33::SetColumn )
		.def("TransformByTranspose", &Matrix33::TransformByTranspose )
		.def("SetRotationX",         &Matrix33::SetRotationX )
		.def("SetRotationY",         &Matrix33::SetRotationY )
		.def("SetRotationZ",         &Matrix33::SetRotationZ )
	;

	def( "Matrix33Identity",  Matrix33IdentityForBoostPython );
	def( "Matrix33RotationX", Matrix33RotationX );
	def( "Matrix33RotationY", Matrix33RotationY );
	def( "Matrix33RotationZ", Matrix33RotationZ );

	class_<Matrix34>("Matrix34")
		.def(init<Vector3,Matrix33>())
		.def("SetIdentity",  &Matrix34::Identity )
		.def(self * self)
		.def(self * Vector3())
	;

	def( "Matrix34Identity", Matrix34Identity );

//	def( Matrix34() * Vector3() );

	class_<AABB3>("AABB3")
		.def(init<Vector3,Vector3>())
		.def("Nullify",            &AABB3::Nullify )
		//.def("SetMinMax",        &AABB3::SetMinMax )
		.def("GetCenterPosition",  &AABB3::GetCenterPosition )
		.def("GetExtents",         &AABB3::GetExtents )
		.def("AddPoint",           &AABB3::AddPoint )
		.def("AddSphere",          &AABB3::AddSphere )
		.def("AddSphere",          &AABB3::AddSphere )
		.def("Merge2AABBs",        &AABB3::Merge2AABBs )
		.def("MergeAABB",          &AABB3::MergeAABB )
		.def("GetVolume",          &AABB3::GetVolume )
		.def("IsIntersectingWith", &AABB3::IsIntersectingWith )
		.def("IsPointInside",      &AABB3::IsPointInside )
	;

	class_<AABB2>("AABB2")
		.def(init<Vector2,Vector2>())
		.def("Nullify",            &AABB2::Nullify )
		//.def("SetMinMax",        &AABB2::SetMinMax )
		.def("GetCenterPosition",  &AABB2::GetCenterPosition )
		.def("GetExtents",         &AABB2::GetExtents )
		.def("AddPoint",           &AABB2::AddPoint )
		.def("Merge2AABBs",        &AABB2::Merge2AABBs )
		.def("MergeAABB",          &AABB2::MergeAABB )
		.def("GetArea",            &AABB2::GetArea )
		.def("IsIntersectingWith", &AABB2::IsIntersectingWith )
		.def("IsPointInside",      &AABB2::IsPointInside )
	;
}


void RegisterPythonModule_math3d()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("math3d", initmath3d) == -1)
	{
		const char *msg = "Failed to add math3d to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}
