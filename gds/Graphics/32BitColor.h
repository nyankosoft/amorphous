
#ifndef __32BITCOLOR_H__
#define __32BITCOLOR_H__


struct S32BitColor
{
	/// color (r,g,b,a)
	unsigned char color[4];

	unsigned char& red()   { return color[0]; }
	unsigned char& green() { return color[1]; }
	unsigned char& blue()  { return color[2]; }
	unsigned char& alpha() { return color[3]; }

	unsigned char& operator[](int i) { return color[i]; }

	S32BitColor() {}

	S32BitColor( unsigned char r,  unsigned char g,  unsigned char b,  unsigned char a )
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}
//		: color[0](r), color[1](g), color[2](b), color[3](a) {}
};


#endif  /*  __32BITCOLOR_H__  */