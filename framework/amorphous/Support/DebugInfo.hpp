#ifndef __DebugInfo_HPP__
#define __DebugInfo_HPP__


#include <vector>
#include <string>
#include "amorphous/Graphics/FloatRGBAColor.hpp"


namespace amorphous
{


/**
  \brief Used to store debug information
  Derived classes are responsible for implementing UpdateDebugInfoText()
  to update the content of either m_TextBuffer or m_MultiLineTextBuffer.
*/
class DebugInfo
{
protected:

	std::string m_Name;

	std::string m_TextBuffer;

	std::vector<std::string> m_MultiLineTextBuffer;

	std::vector<SFloatRGBAColor> m_LineColors;

public:

	DebugInfo() {}

	virtual ~DebugInfo() {}

	virtual void SetName( std::string name ) { m_Name = name; }
	const std::string& GetName() const { return m_Name; }

	const std::string& GetTextBuffer() const { return m_TextBuffer; }

	const std::vector<std::string> GetMultiLineTextBuffer() const { return m_MultiLineTextBuffer; }

	const std::vector<SFloatRGBAColor> GetMultiLineTextColors() const { return m_LineColors; }

	virtual void UpdateDebugInfoText() = 0;
};


} // namespace amorphous


#endif /* __DebugInfo_HPP__ */
