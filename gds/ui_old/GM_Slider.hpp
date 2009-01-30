
#ifndef  __GM_SLIDER_H__
#define  __GM_SLIDER_H__

#include "GM_Control.hpp"
#include "GM_GraphicElement.hpp"


//========================================================================================
// CGM_Slider
//========================================================================================

class CGM_SliderDesc;
struct SInputData;

class CGM_Slider : public CGM_Control
{
protected:

    void SetValueInternal( int iValue, bool bFromInput );

    int ValueFromPos( int x, int y ); 

    int m_iValue;

    int m_iMin;
    int m_iMax;

    int m_iDragX;      // Mouse position at start of drag
    int m_iDragOffset; // Drag offset from the center of the button
    int m_iButtonX;

    bool m_bPressed;

	CGM_TextureRectElement m_BoundingRectElement;
	CGM_TextureRectElement m_ButtonElement;

    SRect m_ButtonRect;

public:
	CGM_Slider( CGM_Dialog *pDialog, CGM_SliderDesc *pDesc );
	~CGM_Slider() {}

	virtual bool IsPointInside( SPoint& pt ); 
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
//    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual bool HandleMouseInput( SInputData& input );
    
    virtual void UpdateRects(); 

    virtual void Render( float fElapsedTime );

    void SetValue( int iValue ) { SetValueInternal( iValue, false ); }

    int  GetValue() { return m_iValue; };

    void SetRange( int iMin, int iMax );

	int ValueFromPos( int x );

};


#endif  /*  __GM_SLIDER_H__  */