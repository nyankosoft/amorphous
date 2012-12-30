#include "EmbeddedPythonModule.hpp"


namespace amorphous
{


static const CEmbeddedPythonModule g_EmbeddedPythonModule_math3d = CEmbeddedPythonModule(
"math3d",
"import math\n"\
"\n"\
"\n"\
"class Vector2:\n"\
"	x=0\n"\
"	y=0\n"\
"\n"\
"	def __init__(self):\n"\
"		x=0\n"\
"		y=0\n"\
"\n"\
"	def GetLength(self):\n"\
"		return math.sqrt(self.x*self.x+self.y*self.y)\n"\
"\n"\
"	def Normalize(self):\n"\
"		len = self.length()\n"\
"		self.x = self.x / len\n"\
"		self.y = self.y / len\n"\
"\n"\
"	def __add__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x + other.x\n"\
"		out.y = self.y + other.y\n"\
"		return out\n"\
"\n"\
"	def __sub__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x - other.x\n"\
"		out.y = self.y - other.y\n"\
"		return out\n"\
"\n"\
"	def __mul__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x * other\n"\
"		out.y = self.y * other\n"\
"		return out\n"\
"\n"\
"	def __div__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x / other\n"\
"		out.y = self.y / other\n"\
"		return out\n"\
"\n"\
"\n"\
"class Vector3:\n"\
"	x=0\n"\
"	y=0\n"\
"	z=0\n"\
"\n"\
"	def __init__(self):\n"\
"		x=0\n"\
"		y=0\n"\
"		z=0\n"\
"\n"\
"	def GetLength(self):\n"\
"		return math.sqrt(self.x*self.x+self.y*self.y+self.z*self.z)\n"\
"\n"\
"	def Normalize(self):\n"\
"		len = self.length()\n"\
"		self.x = self.x / len\n"\
"		self.y = self.y / len\n"\
"		self.z = self.z / len\n"\
"\n"\
"	def __add__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x + other.x\n"\
"		out.y = self.y + other.y\n"\
"		out.z = self.z + other.z\n"\
"		return out\n"\
"\n"\
"	def __sub__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x - other.x\n"\
"		out.y = self.y - other.y\n"\
"		out.z = self.z - other.z\n"\
"		return out\n"\
"\n"\
"	def __mul__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x * other\n"\
"		out.y = self.y * other\n"\
"		out.z = self.z * other\n"\
"		return out\n"\
"\n"\
"	def __div__(self,other):\n"\
"		out = Vector3()\n"\
"		out.x = self.x / other\n"\
"		out.y = self.y / other\n"\
"		out.z = self.z / other\n"\
"		return out\n"\
"\n"\
"def Vec3Dot( lhs, rhs ):\n"\
"	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z\n"\
"\n"\
"def Vec3Cross( lhs, rhs ):\n"\
"	out = Vector3()\n"\
"	out.x = lhs.y * rhs.z - lhs.z * rhs.y\n"\
"	out.y = lhs.z * rhs.x - lhs.x * rhs.z\n"\
"	out.z = lhs.x * rhs.y - lhs.y * rhs.x\n"\
"	return out\n"\
"\n"\
"\n"\
"class Plane:\n"\
"	normal = Vector3()\n"\
"	dist = 0\n"\
"\n"\
"	def __init__(self):\n"\
"		normal = Vector3()\n"\
"		dist = 0\n"\
"\n"\
"	def GetDistanceFromPoint(self,point):\n"\
"		return Vec3Dot(self.normal,point) - self.dist\n"\
"\n"\
"\n"\
"class AABB3:\n"\
"	min = Vector3()\n"\
"	max = Vector3()\n"\
"\n"\
"	def __init__(self):\n"\
"		min = Vector3()\n"\
"		max = Vector3()\n"\
"\n"\
"	def GetCenterPosition(self):\n"\
"		return (self.min + self.max) * 0.5\n"\
"\n"\
"	def GetVolume(self):\n"\
"		return (self.max.x - self.min.x) * (self.max.y - self.min.y) * (self.max.z - self.min.z)\n"\
"\n"\
"	def IsIntersectingWith(self,other):\n"\
"		if self.max.x < other.min.x or other.max.x < self.min.x or self.max.y < other.min.y or other.max.y < self.min.y or self.max.z < other.min.z or other.max.z < self.min.z:\n"\
"			return False\n"\
"		else:\n"\
"			return True\n"\
"\n"\
"	def IsPointInside(self,other):\n"\
"		if self.min.x <= other.x and other.x <= self.max.x and self.min.y <= other.y and other.y <= self.max.y and self.min.z <= other.z and other.z <= self.max.z:\n"\
"			return True\n"\
"		else:\n"\
"			return False\n"
);


const CEmbeddedPythonModule& GetEmbeddedPythonModule_math3d() { return g_EmbeddedPythonModule_math3d; }


} // namespace amorphous
