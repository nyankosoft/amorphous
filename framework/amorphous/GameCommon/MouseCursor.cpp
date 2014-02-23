#include "MouseCursor.hpp"
#include "amorphous/Graphics/GraphicsElementManager.hpp"


namespace amorphous
{


singleton<MouseCursor> MouseCursor::m_obj;


void MouseCursor::Show()
{
	m_bDisplay = true;

	if( m_pCursor )
		m_pCursor->SetColor( 1, SFloatRGBAColor(1,1,1,1) );
}


void MouseCursor::Hide()
{
	m_bDisplay = false;

	if( m_pCursor )
		m_pCursor->SetColor( 1, SFloatRGBAColor(0,0,0,0) );
}


void MouseCursor::Load( GraphicsElementManager& mgr, int index, const SRect& rect, const std::string& tex_filename, const SFloatRGBAColor& color )
{
	int layer = 0;

	m_pCursor = mgr.CreateFillRect(
		rect,
		color,
		layer
		);
}


void MouseCursor::UpdateCursorPosition( int x, int y )
{
	m_PosX = x;
	m_PosY = y;

	if( m_pCursor )
		m_pCursor->SetLocalTopLeftPos( SPoint(x,y) );
}


void MouseCursor::CreateDefault( GraphicsElementManager& mgr )
{
	Vector2 v0( 0, 0);
	Vector2 v1(20,15);
	Vector2 v2(15,20);

	m_pCursor = mgr.CreateFillTriangle( v0, v1, v2, SFloatRGBAColor::White() );
}


void MouseCursor::ReleaseGraphicsResources( GraphicsElementManager& mgr )
{
	bool res = mgr.RemoveElement( m_pCursor );

	// Release texture(s) registered to the graphics element manager?
}


} // namespace amorphous
