#ifndef __DebugInfo_GraphicsResourceManager_HPP__
#define __DebugInfo_GraphicsResourceManager_HPP__


#include "amorphous/Support/DebugInfo.hpp"
#include "GraphicsResourceManager.hpp"


namespace amorphous
{


class DebugInfo_GraphicsResourceManager : public DebugInfo
{
	std::vector<std::string> m_vecTextBuffer;

public:

	DebugInfo_GraphicsResourceManager() {}

	virtual void UpdateDebugInfoText()
	{
		m_TextBuffer.resize( 0 );
		GetGraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

		// Debug info of graphics resource tends to grow too big.
		// For now, we just cut them short because they will not fit to the screen anyway.
		// TODO: support scrolling mechanism to let developers view the entire content.
		const int max_buffer_size = 2048;
		m_TextBuffer.resize( max_buffer_size );
	}
};


} // namespace amorphous


#endif /* __DebugInfo_GraphicsResourceManager_HPP__ */
