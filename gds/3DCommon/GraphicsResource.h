#ifndef  __GraphicsResource_H__
#define  __GraphicsResource_H__


#include <string>


class GraphicsResourceType
{
public:
	enum Name
	{
		Texture,
		Mesh,
		Shader,
		Font,
		NumTypes
	};
};


class GraphicsResourceFlag
{
public:
	enum Flags
	{
		DontShare = ( 1 << 0 ),
		Flag1     = ( 1 << 1 ),
		Flag2     = ( 1 << 2 ),
	};
};


/**
 request to load a resource
*/
class ResourceLoadRequest
{
public:

	std::string name;
	GraphicsResourceType::Name type;
};



#endif /* __GraphicsResource_H__ */