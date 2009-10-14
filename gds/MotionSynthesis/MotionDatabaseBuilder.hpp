#ifndef  __MotionDatabaseBuilder_H__
#define  __MotionDatabaseBuilder_H__


#include <sys/stat.h>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "XML/XMLNodeReader.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;

#include "fwd.hpp"
#include "MotionPrimitive.hpp"
#include "MotionDatabase.hpp"
#include "Skeleton.hpp"
#include "HumanoidMotionTable.hpp"


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
	/// or .lws file
	std::string m_Filename;

	/// skeleton extracted from the motion file
	/// - each compiler implementation is responsible for creating a skeleton
	///   from motion data store it here.
	/// - BVH file: create a skeleton from the HIERARCHY section of BVH file
	/// - LWS file: create a skeleton from bones in LWS file
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


class CMotionDatabaseCompiler
{
protected:

	/// Borrowed reference set by CMotionDatabaseBuilder and available to derived class
	/// derived classes are responsible for storing created motion primitives to this variable
	std::vector<CMotionPrimitive> *m_vecpMotionPrimitive;

	std::vector<std::string> *m_vecpAnnotationName;

	CHumanoidMotionTable *m_pMotionTable;

protected:

	/// Allows derived class to access private member of 'CMotionPrimitive'
	std::vector<char>& AnnotationArray( CMotionPrimitive& motion ) { return motion.m_vecAnnotation; }

	int GetAnnotationIndex( const std::string& annotation_name );

public:

	CMotionDatabaseCompiler()
		:
	m_vecpMotionPrimitive(NULL),
	m_vecpAnnotationName(NULL),
	m_pMotionTable(NULL)
	{}

	virtual ~CMotionDatabaseCompiler() {}

	/// Create motion data from descs
	/// - Each derived class is responsible for implementing this method
	///   to create motion primitives from 'm_vecDescGroup' and store
	///   them to 'm_vecMotionPrimitive'
	virtual void CreateMotionPrimitives( CMotionPrimitiveDescGroup& desc_group ) = 0;

	friend CMotionDatabaseBuilder;
};


class CMotionPrimitiveCompilerCreator
{
public:

	CMotionPrimitiveCompilerCreator() {}

	virtual ~CMotionPrimitiveCompilerCreator() {}

	virtual const char *Extension() const = 0;

	virtual boost::shared_ptr<CMotionDatabaseCompiler> Create() const = 0;
};


/**
 - input: xml files that contain motion primitive descs
 - output: motion database file


*/
class CMotionDatabaseBuilder
{
protected:

	std::vector<CMotionPrimitiveDescGroup> m_vecDescGroup;

	/// stores motion primitives created from descs above
	std::vector<CMotionPrimitive> m_vecMotionPrimitive;

	std::vector<std::string> m_vecAnnotationName;

	CHumanoidMotionTable m_MotionTable;

	std::string m_SourceXMLFilename;

	std::string m_OutputFilepath;

protected:

//	int GetAnnotationIndex( const std::string& annotation_name );

	void CreateMotionPrimitiveDesc( xercesc::DOMNode *pMotionNode );

	/// called once for every bvh file
	void CreateMotionPrimitiveDescGroup( xercesc::DOMNode *pRootNode );

//	void CreateMotionPrimitive( const CMotionPrimitiveDesc& desc, const CMotionPrimitiveDescGroup& desc_group, CBVHPlayer& bvh_player );

	void CreateMotionTableEntry( xercesc::DOMNode *pMotionEntryNode, CHumanoidMotionEntry& entry );

	void CreateMotionTable( xercesc::DOMNode *pMotionTableNode );

	void ProcessXMLFile( CXMLNodeReader& root_node );
//	void ProcessXMLFile( xercesc::DOMNode *pRootNode );

	bool CreateMotionPrimitivesFromScriptFile( const std::string& script_filename );

	bool CreateAnnotationTable( CXMLNodeReader& annot_table_node );
//	bool CreateAnnotationTable( xercesc::DOMNode *pAnnotTableNode );

	void ProcessRootNodeHorizontalElementOptions( xercesc::DOMNode *RootJointNode, CMotionPrimitiveDesc& desc );

//	void ProcessFiles();

	void CreateMotionPrimitives();

public:

	CMotionDatabaseBuilder() {}

	~CMotionDatabaseBuilder() {}

	bool Build( const std::string& source_script_filename );

	bool SaveMotionDatabaseToFile( const std::string& db_filename );

	virtual bool IsValidMotionFile( const std::string& src_filepath ) { return true; }

	const std::string& GetOutputFilepath() const { return m_OutputFilepath; }
};


/// Used by derived classes
extern void AlignLastKeyframe( std::vector<CKeyframe>& vecKeyframe );


extern void RegisterMotionPrimitiveCompilerCreator( boost::shared_ptr<CMotionPrimitiveCompilerCreator> pCreator );


} // namespace msynth


#endif /* __MotionDatabaseBuilder_H__ */
