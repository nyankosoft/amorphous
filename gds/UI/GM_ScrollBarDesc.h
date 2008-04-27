#ifndef  __GM_SCROLLBARDESC_H__
#define  __GM_SCROLLBARDESC_H__

#include "GM_ControlDescBase.h"


class CGM_Dialog;

class CGM_ScrollBarDesc : public CGM_ControlDesc
{
public:

	std::string strText;

	int iPageSize;

	int iInitPosition;

public:

	CGM_ScrollBarDesc() { SetDefault(); }
	virtual ~CGM_ScrollBarDesc() {}

	virtual void SetDefault()
	{
		iPageSize = 1;
		iInitPosition = 0;
	}

	virtual unsigned int GetType() { return CGM_Control::SCROLLBAR; }
};


#endif		/*  __GM_SCROLLBARDESC_H__  */
