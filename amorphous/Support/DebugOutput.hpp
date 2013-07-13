#ifndef  __DEBUGOUTPUT_H__
#define  __DEBUGOUTPUT_H__


#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "singleton.hpp"  // Used by CGlobalDebugOutput
#include "DebugInfo.hpp"


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


/// A concrete class in Graphics module
class OnScreenDebugInfoRendererBase
{
public:
	OnScreenDebugInfoRendererBase(){}
	virtual ~OnScreenDebugInfoRendererBase(){}

	virtual void RenderDebugInfo( const std::string& debug_info_text ) = 0;

	virtual void RenderDebugInfo(
		const std::vector<std::string>& debug_info_texts,
		const std::vector<SFloatRGBAColor> text_colors
		) = 0;
};


class CDebugOutput
{
	int m_ItemIndex;

	/// owned reference
	std::vector<DebugInfo *> m_vecpDebugItem;

	bool m_bDisplay;

	/// top left corner position of the text
//	Vector2 m_vTopLeftPos;

	std::unique_ptr<OnScreenDebugInfoRendererBase> m_pRenderer;

public:

	CDebugOutput();

	~CDebugOutput();

	void Release();

	void Render();

	void AddDebugItem( const std::string& item_name, DebugInfo *pDebugItem );
	bool ReleaseDebugItem( const std::string& item_name );

	void SetDebugItemIndex( int index );
	void PrevDebugItem() { SetPrevIndex( m_ItemIndex, m_vecpDebugItem ); }
	void NextDebugItem() { SetNextIndex( m_ItemIndex, m_vecpDebugItem ); }

	/// Returns true on success
	bool SetDebugItem( const std::string& item_name );

	void Show() { m_bDisplay = true; }
	void Hide() { m_bDisplay = false; }
	void ToggleDisplay() { m_bDisplay = !m_bDisplay; }

//	void SetTopLeftPos( Vector2& vTopLeftPos );

	void SetOnScreenDebugInfoRendererBase( OnScreenDebugInfoRendererBase *pRenderer ) { m_pRenderer.reset( pRenderer ); }
};


class LogOutput_ScrolledTextBuffer;


class DebugInfo_Log : public DebugInfo
{
	/// borrowed reference
	LogOutput_ScrolledTextBuffer *m_pLogOutput;

public:

//	DebugInfo_Log() {}

	DebugInfo_Log( LogOutput_ScrolledTextBuffer* pLogOutput );

	void UpdateDebugInfoText();
};


class DebugInfo_Profile : public DebugInfo
{
public:

	DebugInfo_Profile() {}

	void UpdateDebugInfoText();
};


class DebugInfo_StateLog : public DebugInfo
{
public:

	DebugInfo_StateLog() {}

	void UpdateDebugInfoText();
};


#define GlobalDebugOutput (*CGlobalDebugOutput::Get())
#define DebugOutput (*(CGlobalDebugOutput::Get()->GetGlobalInstance()))

class CGlobalDebugOutput
{
	static singleton<CGlobalDebugOutput> m_obj;	///< singleton instance

	std::unique_ptr<CDebugOutput> m_pDebugOutput;

public:

	static CGlobalDebugOutput* Get() { return m_obj.get(); }

	CGlobalDebugOutput()	///< ctor needs to be public?
	{
	}

	void Init( const std::string& font_name, int w, int h )
	{
		m_pDebugOutput.reset( new CDebugOutput() );
	}

	void Release()
	{
		m_pDebugOutput.reset();
	}

	CDebugOutput *GetGlobalInstance()
	{
		return m_pDebugOutput.get();
	}
};

} // namespace amorphous



#endif  /* __DEBUGOUTPUT_H__ */
