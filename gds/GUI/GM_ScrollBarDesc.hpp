#ifndef  __GM_SCROLLBARDESC_H__
#define  __GM_SCROLLBARDESC_H__

#include "fwd.hpp"
#include "GM_ControlDescBase.hpp"


namespace amorphous
{


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

} // namespace amorphous



#endif		/*  __GM_SCROLLBARDESC_H__  */
