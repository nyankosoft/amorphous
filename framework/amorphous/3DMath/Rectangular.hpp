//=====================================================================
//  Rectangular.hpp
//     - generic 2-dimensional class
//
//                                                  by Takashi Kuamgai
//=====================================================================


#ifndef  __Rectangular_HPP__
#define  __Rectangular_HPP__


template<typename T>
class tRectangular
{
public:

	T width, height;

	tRectangular()
		:
	width(0),
	height(0)
	{}

	tRectangular( T _width, T _height )
		:
	width(_width),
	height(_height)
	{}

	T GetArea() const { return width * height; }

};


typedef tRectangular<float> Rectangular;
typedef tRectangular<double> dRectangular;
typedef tRectangular<int> SRectangular;



#endif		/*  tRectangular  */
