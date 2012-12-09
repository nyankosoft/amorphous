#include "EmbeddedPythonModule.hpp"


static const CEmbeddedPythonModule g_EmbeddedPythonModule_gfx = CEmbeddedPythonModule(
"gfx",
"import math\n"\
"\n"\
"\n"\
"class RGBAColor:\n"\
"	red   = 0\n"\
"	green = 0\n"\
"	blue  = 0\n"\
"	alpha = 1\n"\
"\n"\
"	def __init__(self):\n"\
"		red   = 0\n"\
"		green = 0\n"\
"		blue  = 0\n"\
"		alpha = 1\n"\
"\n"\
"	def __add__(self,other):\n"\
"		out = RGBAColor()\n"\
"		out.red   = self.red   + other.red\n"\
"		out.green = self.green + other.green\n"\
"		out.blue  = self.blue  + other.blue\n"\
"		out.alpha = self.alpha + other.alpha\n"\
"		return out\n"\
"\n"\
"	def __sub__(self,other):\n"\
"		out = RGBAColor()\n"\
"		out.red   = self.red   - other.red\n"\
"		out.green = self.green - other.green\n"\
"		out.blue  = self.blue  - other.blue\n"\
"		out.alpha = self.alpha - other.alpha\n"\
"		return out\n"\
"\n"\
"	def __mul__(self,other):\n"\
"		out = RGBAColor()\n"\
"		out.red   = self.red   * other.red\n"\
"		out.green = self.green * other.green\n"\
"		out.blue  = self.blue  * other.blue\n"\
"		out.alpha = self.alpha * other.alpha\n"\
"		return out\n"\
""
//"\n"\
//"	Red = RGBAColor(1,0,0,1)\n"\
//"\n"\
//"	def White():\n"\
//"		out = RGBAColor(1,1,1,1)\n"\
//"		return out\n"\
//""
);


const CEmbeddedPythonModule& GetEmbeddedPythonModule_gfx() { return g_EmbeddedPythonModule_gfx; }
