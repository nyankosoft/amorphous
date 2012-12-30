#ifndef  __CORRELATIONGRAPH_H__
#define  __CORRELATIONGRAPH_H__


#include "2DGraph.hpp"


namespace amorphous
{


class CCorrelationGraph : public C2DGraph
{
public:
	void SetData(vector<float> *pvecfData, U32 dwColor = 0xFF00BB00);
	void Draw();
	void SetPosition(float sx, float ex, float sy, float ey);

};

} // amorphous



#endif  /*  __CORRELATIONGRAPH_H__  */
