
#ifndef __STAGEGRAPH_SINGLETON_H__
#define __STAGEGRAPH_SINGLETON_H__


#include "Support/Singleton.h"
using namespace NS_KGL;

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

#include <Python.h>


extern PyMethodDef g_PyModuleStageGraphMethod[];


#define StageGraph	(*(CStageGraph::Get()))



class StageNodeDesc
{
public:
	std::string base_node_name;
	std::string name;
	std::string script_filename;
//	Vector3 vWorldMapPos;
};


class StageNode : public IArchiveObjectBase
{
public:
	
	enum eStatusFlag
	{
		CLEARED		= (1 << 0),
		AVAILABLE	= (1 << 1),
	};

private:

	enum params
	{
		NUM_MAX_DIVS = 4
	};

	std::string m_Name;

	std::string m_StageScriptFilename;

//	bool m_Cleared;
	int m_StateFlag;

//	std::vector<StageNode> vecNextMission;

	StageNode *m_apNextStage[NUM_MAX_DIVS];
	int m_NumNextStages;

	StageNode *m_pPrevStage;

public:

//	StageNode() : {}
//	virtual ~StageNode() {}

	StageNode() : m_StateFlag(AVAILABLE), m_NumNextStages(0), m_pPrevStage(NULL) { memset( m_apNextStage, 0, sizeof(m_apNextStage) ); }

	StageNode( const StageNodeDesc& desc );

	virtual ~StageNode() { Release(); }

	void Release();

	const std::string& GetName() { return m_Name; }

	const std::string& GetStageScriptFilename() { return m_StageScriptFilename; }

	void AddNode( const StageNodeDesc& desc );

	StageNode *GetNode_r( const std::string& name );

	void SetState( int state ) { m_StateFlag = state; }

	int GetState() const { return m_StateFlag; }

	void GetAvailableStages_r( std::vector<StageNode *>& rvecStage/*, int search_cond*/ );

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Name;
		ar & m_StageScriptFilename;

		if( ar.GetMode() == IArchive::MODE_INPUT )
		{
			Release();
			ar & m_NumNextStages;
			for( int i=0; i<m_NumNextStages; i++ )
				m_apNextStage[i] = new StageNode();
		}
		else
			ar & m_NumNextStages;

		for( int i=0; i<m_NumNextStages; i++ )
			ar & (*m_apNextStage[i]);
	}


	friend class CGameTask_MainMenuFG;
};


class CStageGraph// : public IArchiveObjectBase
{
	StageNode* m_pStageTree;

public:

	static CSingleton<CStageGraph> m_obj;

	static CStageGraph* Get() { return m_obj.get(); }

	CStageGraph() : m_pStageTree(NULL) {}

	~CStageGraph() { Release(); }

	StageNode *GetRootNode() { return m_pStageTree; }

	void AddStageNode( const StageNodeDesc& desc );

	void Release();

	void MarkStageNodeAsCleared( const std::string& stage_node_name );

//	bool Update( const char *pcFilename );

	void GetAvailableStages( std::vector<StageNode *>& rvecStage/*, int search_cond*/ );

	bool LoadFromFile( const std::string& filename );

};


#endif  /*  __STAGEGRAPH_SINGLETON_H__  */
