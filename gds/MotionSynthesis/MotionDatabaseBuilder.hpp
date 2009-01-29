#ifndef  __MotionDatabaseBuilder_H__
#define  __MotionDatabaseBuilder_H__


#include <sys/stat.h>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/BinaryDatabase.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

#include "fwd.h"
#include "MotionPrimitive.h"
#include "MotionDatabase.h"
#include "Skeleton.h"
#include "HumanoidMotionTable.h"


namespace msynth
{


class CMotionPrimitiveDesc
{
public:

	std::string m_Name;
	int m_StartFrame;
	int m_EndFrame;

	bool m_bIsLoopMotion;

	bool m_bResetHorizontalRootPos;

	/// how to normalize horizontal orientation of the motion
	/// AverageMotionDirection or
	/// LocalPositiveZDirection or
	/// AlignLastKeyframe - put that last keyframe on the x=0 axis
	/// None
	std::string m_NormalizeOrientation;

	std::vector<std::string> m_vecAnnotation;

public:

	CMotionPrimitiveDesc()
		:
	m_StartFrame(0),
	m_EndFrame(0),
	m_bIsLoopMotion(false),
	m_bResetHorizontalRootPos(false)
	{}

	CMotionPrimitiveDesc( int sf, int ef ) : m_StartFrame(sf), m_EndFrame(ef), m_bIsLoopMotion(false) {}
};


/// created for each bvh file
class CMotionPrimitiveDescGroup
{
public:

	/// .bvh file that contains captured motion data
	std::string m_Filename;

	/// skeleton extracted from the bvh file
	CSkeleton m_Skeleton;

	/// scaling factor applied to skeleton and all the motion primitives
	/// created from file
	float m_fScalingFactor;

	std::vector<CMotionPrimitiveDesc> m_Desc;

public:

	CMotionPrimitiveDescGroup()
		:
	m_fScalingFactor(1.0f)
	{}
};


/**
 - input: xml files that contain motion primitive descs
 - output: motion database file


*/
class CMotionDatabaseBuilder
{
	std::vector<CMotionPrimitiveDescGroup> m_vecDescGroup;

	/// stores motion primitives created from descs above
	std::vector<CMotionPrimitive> m_vecMotionPrimitive;

	std::vector<std::string> m_vecAnnotationName;

	CHumanoidMotionTable m_MotionTable;

private:

	int GetAnnotationIndex( const std::string& annotation_name );

	// create motion data from descs
	void CreateMotionPrimitives();

	void CreateMotionPrimitiveDesc( xercesc_2_8::DOMNode *pMotionNode );

	/// called once for every bvh file
	void CreateMotionPrimitiveDescGroup( xercesc_2_8::DOMNode *pRootNode );

	void CreateMotionPrimitive( const CMotionPrimitiveDesc& desc, const CMotionPrimitiveDescGroup& desc_group, CBVHPlayer& bvh_player );

	void CreateMotionTableEntry( xercesc_2_8::DOMNode *pMotionEntryNode, CHumanoidMotionEntry& entry );

	void CreateMotionTable( xercesc_2_8::DOMNode *pMotionTableNode );

	void ProcessXMLFile( xercesc_2_8::DOMNode *pFileNode );

	bool CreateMotionPrimitivesFromScriptFile( const std::string& script_filename );

	bool CreateAnnotationTable( xercesc_2_8::DOMNode *pAnnotTableNode );

	void ProcessRootNodeHorizontalElementOptions( xercesc_2_8::DOMNode *RootJointNode, CMotionPrimitiveDesc& desc );

//	void ProcessFiles();

public:

	CMotionDatabaseBuilder() {}

	~CMotionDatabaseBuilder() {}

	bool Build( const std::string& source_script_filename );

	bool SaveMotionDatabaseToFile( const std::string& db_filename );
};


} // namespace msynth


#endif /* __MotionDatabaseBuilder_H__ */