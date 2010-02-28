#ifndef  __VertexFormat_HPP__
#define  __VertexFormat_HPP__


/// Vertex format flags
class VFF
{
public:
	enum Flags
	{
		POSITION      = (1 << 0),
		NORMAL        = (1 << 1),
		BINORMAL      = (1 << 2),
		TANGENT       = (1 << 3),
		DIFFUSE_COLOR = (1 << 4),
		TEXCOORD2_0   = (1 << 5),
		TEXCOORD2_1   = (1 << 6),
		TEXCOORD2_2   = (1 << 7),
		TEXCOORD2_3   = (1 << 8),
	};
};

/// Vertex element enum
class VEE
{
public:
	enum ElementName
	{
		POSITION = 0,
		NORMAL,
		BINORMAL,
		TANGENT,
		DIFFUSE_COLOR,
		TEXCOORD2_0,
		TEXCOORD2_1,
		TEXCOORD2_2,
		TEXCOORD2_3,
		NUM_VERTEX_ELEMENTS
	};
};


#endif /* __VertexFormat_HPP__ */
