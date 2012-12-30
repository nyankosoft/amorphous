#ifndef __BVHMotionDatabaseCompiler_HPP__
#define __BVHMotionDatabaseCompiler_HPP__


#include "MotionDatabaseBuilder.hpp"


namespace amorphous
{


namespace msynth
{


class CBVHMotionDatabaseCompiler : public CMotionDatabaseCompiler
{
	std::vector<CMotionPrimitiveDescGroup> m_vecDescGroup;

private:

	bool IsValidMotionFile( const std::string& src_filepath );

	void CreateMotionPrimitive( CMotionPrimitiveDesc& desc, const CMotionPrimitiveDescGroup& desc_group, CBVHPlayer& bvh_player );

	void CreateMotionPrimitives( CMotionPrimitiveDescGroup& desc_group );

public:

};


class CBVHMotionPrimitiveCompilerCreator : public CMotionPrimitiveCompilerCreator
{
public:

	const char *Extension() const { return "bvh"; }

	boost::shared_ptr<CMotionDatabaseCompiler> Create() const
	{
		boost::shared_ptr<CBVHMotionDatabaseCompiler> pCompiler( new CBVHMotionDatabaseCompiler );
		return pCompiler;
	}
};


} // namespace msynth

} // namespace amorphous



#endif /* __BVHMotionDatabaseCompiler_HPP__ */
