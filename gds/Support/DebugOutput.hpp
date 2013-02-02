#ifndef  __DEBUGOUTPUT_H__
#define  __DEBUGOUTPUT_H__


#include <vector>
#include <string>

#include "../base.hpp"
#include "../Graphics/fwd.hpp"
#include "../Graphics/2DPrimitive/2DRect.hpp"
#include "../Graphics/GraphicsComponentCollector.hpp"

#include "SafeDelete.hpp" // Used by CGlobalDebugOutput
#include "Singleton.hpp"  // Used by CGlobalDebugOutput


namespace amorphous
{


template<class T>
inline void SetPrevIndex( int& index, std::vector<T> target )
{
	if( target.size() == 0 )
	{
		// What are we gonna do to this?
//		index = 0;
		return;
	}

	index--;

	if( index < 0 )
		index = (int)target.size() - 1;
}


template<class T>
inline void SetNextIndex( int& index, std::vector<T> target )
{
	if( target.size() == 0 )
	{
		// What are we gonna do to this?
//		index = 0;
		return;
	}

	index++;

	if( (int)target.size() <= index )
		index = 0;
}


template<class T>
inline void ClampIndex( int& index, std::vector<T> target )
{
	if( target.size() == 0 )
	{
		// What are we gonna do to this?
		index = 0;
		return;
	}

	if( index < 0 )
		index = 0;
	else if( (int)target.size() <= index )
		index = (int)target.size() - 1;
}


/*
or,
template<class T>
class vec<T> : public vector<T>
{
public:
	
	void SetNextIndex( int& index ) { index++; if( this->size() <= index ) index = 0; }
};
*/


class CDebugItemBase;


class CDebugOutput : public GraphicsComponent
{
	int m_ItemIndex;

	/// owned reference
	std::vector<CDebugItemBase *> m_vecpDebugItem;

	/// owned reference
	FontBase* m_pFont;

	bool m_bDisplay;

	C2DRect m_BackgroundRect;

	/// top left corner position of the text
	Vector2 m_vTopLeftPos;

private:

	void RenderFPS();

public:

	CDebugOutput( const std::string& font_name, int w, int h, U32 color = 0xFFFFFFFF );
//	CDebugOutput();

	~CDebugOutput();

	void Release();

	void Render();

	void AddDebugItem( const std::string& item_name, CDebugItemBase *pDebugItem );
	bool ReleaseDebugItem( const std::string& item_name );

	void SetDebugItemIndex( int index );
	void PrevDebugItem() { SetPrevIndex( m_ItemIndex, m_vecpDebugItem ); }
	void NextDebugItem() { SetNextIndex( m_ItemIndex, m_vecpDebugItem ); }

	/// Returns true on success
	bool SetDebugItem( const std::string& item_name );

	void Show() { m_bDisplay = true; }
	void Hide() { m_bDisplay = false; }
	void ToggleDisplay() { m_bDisplay = !m_bDisplay; }

	void SetBackgroundRect( const C2DRect& bg_rect ) { m_BackgroundRect = bg_rect; }
	void SetTopLeftPos( Vector2& vTopLeftPos );

	virtual void ReleaseGraphicsResources();
	virtual void LoadGraphicsResources( const GraphicsParameters& rParam );
};


class CDebugItemBase
{
protected:

	std::string m_Name;

	/// borrowed reference
	FontBase* m_pFont;

	Vector2 m_vTopLeftPos;

public:

	CDebugItemBase() : m_pFont(NULL), m_vTopLeftPos(Vector2(0,0)) {}

	virtual ~CDebugItemBase() {}

	virtual void SetFont( FontBase* pFont ) { m_pFont = pFont; }
	virtual void SetName( std::string name ) { m_Name = name; }
	virtual void SetTopLeftPos( Vector2& pos ) { m_vTopLeftPos = pos; }
	const std::string& GetName() const { return m_Name; }

	virtual void Render() = 0;
};


class LogOutput_ScrolledTextBuffer;


class CDebugItem_Log : public CDebugItemBase
{
	/// borrowed reference
	LogOutput_ScrolledTextBuffer *m_pLogOutput;

public:

//	CDebugItem_Log() {}

	CDebugItem_Log( LogOutput_ScrolledTextBuffer* pLogOutput );

	virtual void Render();
};


class CDebugItem_Profile : public CDebugItemBase
{
public:

	CDebugItem_Profile() {}

	virtual void Render();
};


class CDebugItem_StateLog : public CDebugItemBase
{
public:

	CDebugItem_StateLog() {}

	virtual void Render();
};


class CDebugItem_ResourceManager : public CDebugItemBase
{
protected:

	std::string m_TextBuffer;

public:

	CDebugItem_ResourceManager();

	virtual ~CDebugItem_ResourceManager() {}

	void Render();

	virtual void GetTextInfo() = 0;
};


class CDebugItem_GraphicsResourceManager : public CDebugItem_ResourceManager
{
public:

	CDebugItem_GraphicsResourceManager() {}

	void GetTextInfo();
};


class CDebugItem_SoundManager : public CDebugItem_ResourceManager
{
public:

	CDebugItem_SoundManager() {}

	void GetTextInfo();
};


class CDebugItem_InputDevice : public CDebugItem_ResourceManager
{
	std::vector<std::string> m_vecTextBuffer;

public:

	CDebugItem_InputDevice() {}

	void GetTextInfo();
};


#define GlobalDebugOutput (*CGlobalDebugOutput::Get())
#define DebugOutput (*(CGlobalDebugOutput::Get()->GetGlobalInstance()))

class CGlobalDebugOutput
{
	static CSingleton<CGlobalDebugOutput> m_obj;	///< singleton instance

	CDebugOutput *m_pDebugOutput;

public:

	static CGlobalDebugOutput* Get() { return m_obj.get(); }

	CGlobalDebugOutput()	///< ctor needs to be public?
		:
	m_pDebugOutput(NULL)
	{
	}

	void Init( const std::string& font_name, int w, int h )
	{
		m_pDebugOutput = new CDebugOutput( font_name, w, h );
	}

	void Release()
	{
		SafeDelete( m_pDebugOutput );
	}

	CDebugOutput *GetGlobalInstance()
	{
		return m_pDebugOutput;
	}
};

} // namespace amorphous



#endif  /* __DEBUGOUTPUT_H__ */
