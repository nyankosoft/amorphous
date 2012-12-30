#ifndef __BoundingVolumeTreeRenderers_HPP__
#define __BoundingVolumeTreeRenderers_HPP__


namespace amorphous
{

class COBBTree;


void RenderOBBTree( const COBBTree& obb_tree, int draw_level );

} // namespace amorphous


#endif /* __BoundingVolumeTreeRenderers_HPP__ */
