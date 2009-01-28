#ifndef  __STATUSDISPLAYRENDERER_H__
#define  __STATUSDISPLAYRENDERER_H__


class CStatusDisplay;


class CStatusDisplayRenderer
{

public:
	CStatusDisplayRenderer() {};
	~CStatusDisplayRenderer() {};

	virtual void Render( CStatusDisplay* pStatusDisplay ) = 0;

};



#endif		/*  __STATUSDISPLAYRENDERER_H__  */