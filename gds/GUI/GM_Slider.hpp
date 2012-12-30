#ifndef  __GM_SLIDER_H__
#define  __GM_SLIDER_H__


#include "GM_Control.hpp"


namespace amorphous
{


//========================================================================================
// CGM_Slider
//========================================================================================

class CGM_Slider : public CGM_Control
{
public:

	enum InputCode
	{
		IC_INCREASE,
		IC_DECREASE,
		NUM_INPUT_CODES
	};

protected:

	void SetValueInternal( int iValue, bool bFromInput );

	int ValueFromPos( int x, int y ); 

	int m_iValue;

	int m_iMin;
	int m_iMax;

	int m_iDragX;      ///< Mouse position at start of drag
	int m_iDragOffset; ///< Drag offset from the center of the button
	int m_iButtonX;

	bool m_bPressed;

	SRect m_ButtonRect;

	int m_aInputCode[NUM_INPUT_CODES];

	/// how much the values is changed by a single increase/decrease input
	int m_iShiftAmount;

public:

	CGM_Slider( CGM_Dialog *pDialog, CGM_SliderDesc *pDesc );
	~CGM_Slider() {}

	virtual unsigned int GetType() const { return SLIDER; }

	virtual bool IsPointInside( SPoint& pt ); 

	virtual bool CanHaveFocus() const { return (m_bVisible && m_bEnabled); }

	virtual bool HandleMouseInput( CGM_InputData& input );

	virtual bool HandleKeyboardInput( CGM_InputData& input );

	virtual void UpdateRects(); 

	void SetValue( int iValue ) { SetValueInternal( iValue, false ); }

	int  GetValue() { return m_iValue; };

	void SetRange( int iMin, int iMax );

	int ValueFromPos( int x );

	bool IsPressed() const { return m_bPressed; }

	/// Returns the bounding box of the button (global coord)
	const SRect& GetButtonRect() const { return m_ButtonRect; }

	/// Returns the bounding box of the button (local coord of the slider control)
	inline SRect GetLocalButtonRect() const;

	SRect GetLocalButtonRectInOwnerDialogCoord();
};


//=================================== inline implementations ===================================

inline SRect CGM_Slider::GetLocalButtonRect() const
{
	SRect rect = GetButtonRect();
	rect.Offset( -GetBoundingBox().GetTopLeftCorner() );
	return rect;
}

} // namespace amorphous



#endif  /*  __GM_SLIDER_H__  */
