#ifndef __DebugInfo_SoundManager_HPP__
#define __DebugInfo_SoundManager_HPP__


#include "amorphous/Support/DebugInfo.hpp"
#include "SoundManager.hpp"


namespace amorphous
{


class DebugInfo_SoundManager : public DebugInfo
{
public:

	DebugInfo_SoundManager() {}

	void UpdateDebugInfoText()
	{
		m_TextBuffer.resize( 0 );
		GetSoundManager().GetTextInfo( m_TextBuffer );
	}
};


} // namespace amorphous


#endif /* __DebugInfo_SoundManager_HPP__ */
