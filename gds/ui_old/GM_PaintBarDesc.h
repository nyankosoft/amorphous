#ifndef  __GM_CONTROL_PAINTBARDESC_H__
#define  __GM_CONTROL_PAINTBARDESC_H__


#include "GM_ControlDescBase.h"
#include "GM_PaintBar.h"


class CGM_PaintBarDesc : public CGM_ControlDesc
{
public:

	CGM_PaintBarDesc() { m_Type = CGM_Control::PAINTBAR; SetDefault(); }
	~CGM_PaintBarDesc() {}

	unsigned int BarTypeFlag;

	CGM_TextureRectElement *pBarRect;

	virtual void SetDefault();
};



#endif		/*  __GM_CONTROL_PAINTBARDESC_H__  */