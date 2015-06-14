#ifndef __amorphous_MiscShader_HPP__
#define __amorphous_MiscShader_HPP__


namespace amorphous
{


class MiscShader
{
public:
	enum ID
	{
		SINGLE_DIFFUSE_COLOR,
		SHADED_SINGLE_DIFFUSE_COLOR,
		VERTEX_WEIGHT_MAP_DISPLAY,
		DEPTH_RENDERING_IN_VIEW_SPACE,
		DEPTH_RENDERING_IN_PROJECTION_SPACE,
		SKYBOX,
		SINGLE_COLOR_MEMBRANE,
		NUM_IDS
	};
};


} // namespace amorphous


#endif /* __amorphous_MiscShader_HPP__ */
