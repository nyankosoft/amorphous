#include "gtest/gtest.h"
#include "../amorphous/Support/lfs.hpp"
#include "../amorphous/Support/Serialization/Serialization.hpp"
#include "../amorphous/Support/Log/DefaultLogAux.hpp"

using namespace amorphous;

void init_opengl();

#ifdef _DEBUG
#pragma comment( lib, "gtestd-win32.lib")
#else
#pragma comment( lib, "gtest-win32.lib")
#endif

TEST(lfs_test, lfs_Tests) {
  using namespace std;

  lfs::path pathname("a/b/c/d/e/mytextfile.txt");
  lfs::path expected_parent_pathname("a/b/c/d/e");
  lfs::path parent_pathname = pathname.parent_path();

  ASSERT_EQ(parent_pathname,expected_parent_pathname);
  //ASSERT_EQ(other,box);
}

class myarchive : public serialization::IArchiveObjectBase {

	char a;
	int32_t b;
	float c;
	double d;

	std::string s;
	std::vector<int32_t> v;

public:

	myarchive(char _a = 0, int32_t _b = 0, float _c = 0.0f, double _d = 0.0)
		: a(_a), b(_b), c(_c), d(_d) {}

	void Serialize( serialization::IArchive& ar, const unsigned int version ) {
		ar & a & b & c & d;
	}

	void set(const std::string& _s) { s = _s; }
	void set(const std::vector<int32_t>& _v) { v = _v; }

	bool operator==(const myarchive& other) const {
		return a==other.a && b==other.b && c==other.c && d==other.d;
	}
};

TEST(Serialization_test, Serialization_Tests) {

	const char *file_pathname = "myarchive";

	myarchive obj(1,2,3.0f,4.0);
	obj.set(std::string("string data"));
	obj.set(std::vector<int32_t>{1,23,456,7890,12345});
	obj.SaveToFile(file_pathname);

	myarchive other(5,6,7.0f,8.0);
	other.LoadFromFile(file_pathname);

	ASSERT_EQ(obj,other);
}

int main(int argc, char *argv[]) {
  init_opengl();
  testing::InitGoogleTest(&argc,argv);
  InitTextlLog("unittesting.log");
  return RUN_ALL_TESTS();
}
