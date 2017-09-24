#ifndef __BVHMotionDatabaseCompiler_HPP__
#define __BVHMotionDatabaseCompiler_HPP__


#include "MotionDatabaseBuilder.hpp"


namespace amorphous
{


namespace msynth
{


class BVHMotionDatabaseCompiler : public MotionDatabaseCompiler
{
	std::vector<MotionPrimitiveDescGroup> m_vecDescGroup;

private:

	bool IsValidMotionFile( const std::string& src_filepath );

	void CreateMotionPrimitive( MotionPrimitiveDesc& desc, const MotionPrimitiveDescGroup& desc_group, BVHPlayer& bvh_player );

	void CreateMotionPrimitives( MotionPrimitiveDescGroup& desc_group );

public:

};


class BVHMotionPrimitiveCompilerCreator : public MotionPrimitiveCompilerCreator
{
public:

	const char *Extension() const { return "bvh"; }

	std::shared_ptr<MotionDatabaseCompiler> Create() const
	{
		std::shared_ptr<BVHMotionDatabaseCompiler> pCompiler( new BVHMotionDatabaseCompiler );
		return pCompiler;
	}
};


} // namespace msynth

} // namespace amorphous



#endif /* __BVHMotionDatabaseCompiler_HPP__ */
