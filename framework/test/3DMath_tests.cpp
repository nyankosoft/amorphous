#include "gtest/gtest.h"
#include "../amorphous/3DMath/Matrix22.hpp"
#include "../amorphous/3DMath/AABB2.hpp"
#include "../amorphous/3DMath/AABB3.hpp"
#include "../amorphous/3DMath/Quaternion.hpp"
#include "../amorphous/3DMath/Matrix34.hpp"

using namespace amorphous;


static bool is_identity_matrix(const Matrix22& mat) {
	return
		(mat(0, 0) == 1) && (mat(0, 1) == 0)
		&& (mat(1, 0) == 0) && (mat(1, 1) == 1);
}

static bool are_close(const Vector2& lhs, const Vector2& rhs){
	return std::abs(lhs.x - rhs.x) < 0.000001
		&& std::abs(lhs.y - rhs.y) < 0.000001;
}

static bool is_identity_matrix(const Matrix33& mat) {
	return
		(mat(0, 0) == 1) && (mat(0, 1) == 0) && (mat(0, 2) == 0)
		&& (mat(1, 0) == 0) && (mat(1, 1) == 1) && (mat(1, 2) == 0)
		&& (mat(2, 0) == 0) && (mat(2, 1) == 0) && (mat(2, 2) == 1);
}

// tests of Matrix33Rotation* fail with the 1 * 10^-6 tolerance
static bool are_close(const Vector3& lhs, const Vector3& rhs){
	float t = 0.00001f;
	return std::abs(lhs.x - rhs.x) < t
		&& std::abs(lhs.y - rhs.y) < t
		&& std::abs(lhs.z - rhs.z) < t;
}

static bool are_close(const Matrix33& lhs, const Matrix33& rhs){
	float t = 0.00001f;
	return std::abs(lhs(0,0) - rhs(0,0)) < t
		&& std::abs(lhs(0,1) - rhs(0,1)) < t
		&& std::abs(lhs(0,2) - rhs(0,2)) < t
		&& std::abs(lhs(1,0) - rhs(1,0)) < t
		&& std::abs(lhs(1,1) - rhs(1,1)) < t
		&& std::abs(lhs(1,2) - rhs(1,2)) < t
		&& std::abs(lhs(2,0) - rhs(2,0)) < t
		&& std::abs(lhs(2,1) - rhs(2,1)) < t
		&& std::abs(lhs(2,2) - rhs(2,2)) < t;
}

static bool are_close(const Quaternion& lhs, const Quaternion& rhs){
	float t = 0.00001f;
	return std::abs(lhs.x - rhs.x) < t
		&& std::abs(lhs.y - rhs.y) < t
		&& std::abs(lhs.z - rhs.z) < t
		&& std::abs(lhs.w - rhs.w) < t;
}

TEST(Vector2_test, Vector2_Tests) {
	const Vector2 point = Vector2(123,-456);
	const Vector2 other = Vector2(-789,12);

	Vector2 add = point + other;
	ASSERT_EQ(add, Vector2(-666,-444));

	Vector2 sub = point - other;
	ASSERT_EQ(sub, Vector2(912,-468));

	float d = Vec2Dot(point,other);

	ASSERT_EQ(d,-102519.0f);

	ASSERT_EQ(Vec2LengthSq(point),223065.0f);
}

TEST(Matrix22_test, Matrix22_Tests) {

	Matrix22 m( 1, 2, 3, 4 );
	ASSERT_EQ(m.GetColumn(0),Vector2(1,2));
	ASSERT_EQ(m.GetColumn(1),Vector2(3,4));

	Matrix22 a;
	float data[] = { 10, 20, 30, 40 };
	a.SetData(data);
	ASSERT_EQ(a.GetColumn(0),Vector2(10,20));
	ASSERT_EQ(a.GetColumn(1),Vector2(30,40));

	Matrix22 mat1 = Matrix22Identity();
	Matrix22 mat2 = Matrix22Identity();
	Matrix22 mat3 = mat1 * mat2;

	ASSERT_EQ(is_identity_matrix(mat1), true);
	ASSERT_EQ(is_identity_matrix(mat2), true);
	ASSERT_EQ(is_identity_matrix(mat3), true);

	Matrix22 im = Matrix22Identity();
	Vector2 p = Vector2(1,2);
	Vector2 q = im * p;
	ASSERT_EQ(p,q);	

	Vector2 rotated = Matrix22Rotation((float)PI * 0.5f) * Vector2(1,0);
	ASSERT_EQ(are_close(rotated,Vector2(0,1)),true);	
}

TEST(AABB2_test, AABB2_Tests) {

	const AABB2 box = AABB2(Vector2(-12,34),Vector2(56,78));

	ASSERT_EQ(box.GetArea(), 2992.0f);

	ASSERT_EQ(box.GetCenterPosition(), Vector2(22,56));

	auto inside  = Vector2(30,60);
	auto outside = Vector2(300,600);
	ASSERT_EQ(box.IsPointInside(inside),true);
	ASSERT_EQ(box.IsPointInside(outside),false);
}

TEST(Vector3_test, Vector3_Tests) {
	const Vector3 point = Vector3( 1,  -5,  5);
	const Vector3 other = Vector3(20, 20, 3);

	Vector3 add = other + point;
	ASSERT_EQ(add, Vector3(21,15,8));

	Vector3 sub = other - point;
	ASSERT_EQ(sub, Vector3(19,25,-2));

	Vector3 mul = point * 7.0f;
	ASSERT_EQ(mul, Vector3(7,-35,35));

	Vector3 scalar_x_vec = 9.0f * point;
	ASSERT_EQ(scalar_x_vec, Vector3(9,-45,45));

	Vector3 div = point / 5.0f;
	ASSERT_EQ(div, Vector3(0.2f,-1.0f,1.0f));

	ASSERT_EQ(Vec3Dot(point,other),-65);
	ASSERT_EQ(Vec3Cross(point,other),Vector3(-115,97,120));
	//ASSERT_EQ(other,box);

	ASSERT_FLOAT_EQ(Vec3Length(point),std::sqrt(51.0f));

	ASSERT_FLOAT_EQ(Vec3LengthSq(other),809);
}

TEST(Matrix33_test, Matrix33_Tests) {

	Matrix33 m( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
	ASSERT_EQ(m.GetColumn(0),Vector3(1,2,3));
	ASSERT_EQ(m.GetColumn(1),Vector3(4,5,6));
	ASSERT_EQ(m.GetColumn(2),Vector3(7,8,9));

	Matrix33 a;
	float data[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90 };
	a.SetData(data);
	ASSERT_EQ(a.GetColumn(0),Vector3(10,20,30));
	ASSERT_EQ(a.GetColumn(1),Vector3(40,50,60));
	ASSERT_EQ(a.GetColumn(2),Vector3(70,80,90));

	Matrix33 mat1 = Matrix33Identity();
	Matrix33 mat2 = Matrix33Identity();
	Matrix33 mat3 = mat1 * mat2;

	ASSERT_EQ(is_identity_matrix(mat1), true);
	ASSERT_EQ(is_identity_matrix(mat2), true);
	ASSERT_EQ(is_identity_matrix(mat3), true);

	Matrix33 im = Matrix33Identity();
	Vector3 p = Vector3(1,2,3);
	Vector3 q = im * p;
	ASSERT_EQ(p,q);

	Vector3 ry = Matrix33RotationY((float)PI * 0.5f) * Vector3(1,0,0);
	ASSERT_EQ(are_close(ry,Vector3(0,0,-1)),true);	

	Vector3 rz = Matrix33RotationZ((float)PI * 0.5f) * Vector3(30,0,0);
	ASSERT_EQ(are_close(rz,Vector3(0,30,0)),true);	

	Vector3 rx = Matrix33RotationX((float)PI * 0.5f) * Vector3(0,0,100);
	ASSERT_EQ(are_close(rx,Vector3(0,-100,0)),true);	
}

TEST(Quaternion_test, Quaternion_Tests) {

	const Quaternion quat(0,0,0,1);

	// conversion to rotation matrix
	Matrix33 res = quat.ToRotationMatrix();
	ASSERT_EQ(is_identity_matrix(res), true);

	// instance from matrix
	Quaternion ident( Matrix33Identity() );
	ASSERT_EQ(ident.GetLength(),1.0f);
	Matrix33 mat = ident.ToRotationMatrix();
	ASSERT_EQ(is_identity_matrix(mat), true);

	// Test ToRotationMatrix
	Matrix33 a = Matrix33RotationAxis(10.0f, Vec3GetNormalized(Vector3(3,4,5)));
	Quaternion q(a);
	Matrix33 b = q.ToRotationMatrix();
	ASSERT_EQ(are_close(a,b),true);

	// Test FromRotationMatrix
	Quaternion c(Matrix33RotationAxis(20.0f, Vec3GetNormalized(Vector3(3,6,9))));
	Matrix33 m = c.ToRotationMatrix();
	Quaternion d;
	d.FromRotationMatrix(m);
	ASSERT_EQ(are_close(c,d),true);
}

TEST(Matrix34_test, Matrix34_Tests) {

	const Matrix34 pose;

	// Default ctor does not initialize members,
	// and the 3x3 matrix is not initialized by default.
	ASSERT_EQ(pose.vPosition, Vector3(0,0,0));
	//ASSERT_EQ(is_identity_matrix(pose.matOrient), true);

	// identity matrix
	const Matrix34 ident = Matrix34Identity();
	ASSERT_EQ(ident.vPosition, Vector3(0,0,0));
	ASSERT_EQ(is_identity_matrix(ident.matOrient), true);

	// simple multiplication
	const Matrix34 other = Matrix34Identity();
	Matrix34 mul = ident * other;
	ASSERT_EQ(mul.vPosition, Vector3(0,0,0));
	ASSERT_EQ(is_identity_matrix(mul.matOrient), true);

	Matrix34 make_ident;
	make_ident.vPosition = Vector3(12,-34,56);
	make_ident.matOrient = Matrix33RotationAxis(123.0f, Vector3(-1,0,0));
	make_ident.Identity();
	ASSERT_EQ(make_ident,Matrix34Identity());
}

TEST(AABB3_test, AABB3_Tests) {
	const AABB3 box = AABB3(Vector3(-1,-2,-3),Vector3(4,5,6));
	ASSERT_EQ(box.GetVolume(),315);

	AABB3 add_test = box;
	add_test.AddPoint( Vector3(10,11,12) );
	ASSERT_EQ(add_test,AABB3(Vector3(-1,-2,-3),Vector3(10,11,12)));

	const AABB3 other = AABB3(Vector3(10,10,10),Vector3(20,20,20));
	ASSERT_EQ(AABB3(Vector3(-1,-1,-1),Vector3(1,1,1)).IsPointInside(Vector3(0,0,0)),true);
	ASSERT_EQ(AABB3(Vector3(-1,-1,-1),Vector3(1,1,1)).IsIntersectingWith(other),false);
	//ASSERT_EQ(other,box);
}
