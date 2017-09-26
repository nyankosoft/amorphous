#include "2DRectSet.hpp"
#include <memory>


namespace amorphous
{




C2DRectSet::C2DRectSet()
{
	m_pImpl = Ref2DPrimitiveFactory().Create2DRectSetImpl();
}


} // namespace amorphous
