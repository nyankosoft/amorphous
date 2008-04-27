#ifndef  __GM_STATIC_H__
#define  __GM_STATIC_H__

#include "GM_Control.h"

class CGM_StaticDesc;


/**
 *  static control with text
 */
class CGM_Static : public CGM_Control
{
protected:

	std::string m_strText;	///< title or label for the control

public:

	CGM_Static( CGM_Dialog *pDialog, CGM_StaticDesc *pDesc );
	virtual ~CGM_Static() {}

	virtual unsigned int GetType() const { return STATIC; }

//	virtual void Render();

	inline const std::string& GetText() const { return m_strText; }

	void SetText( const std::string& strText );
};


#endif		/*  __GM_STATIC_H__  */
