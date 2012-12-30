
#include "StageGraph.hpp"

#include "3DMath/Vector3.hpp"
#include "Support/memory_helpers.hpp"


namespace amorphous
{


using namespace std;


CSingleton<CStageGraph> CStageGraph::m_obj;


StageNode::StageNode( const StageNodeDesc& desc )
:
m_Name(desc.name),
m_StageScriptFilename(desc.script_filename),
//m_vWorldMapPosition(desc.vWorldMapPos),
m_StateFlag(AVAILABLE),
m_NumNextStages(0),
m_pPrevStage(NULL)
{
	memset( m_apNextStage, 0, sizeof(m_apNextStage) );
}


StageNode *StageNode::GetNode_r( const std::string& name )
{
	if( m_Name == name )
		return this;

	for( int i=0; i<m_NumNextStages; i++ )
	{
		StageNode* pNode = m_apNextStage[i]->GetNode_r( name );
		if( pNode )
			return pNode;
	}

	return NULL;
}


void StageNode::Release()
{
	for( int i=0; i<m_NumNextStages; i++ )
		SafeDelete( m_apNextStage[i] ); 

	m_NumNextStages = 0;
}


void StageNode::AddNode( const StageNodeDesc& desc )
{
	m_apNextStage[m_NumNextStages++] = new StageNode( desc );
	m_apNextStage[m_NumNextStages - 1]->m_pPrevStage = this;
}


void StageNode::GetAvailableStages_r( std::vector<StageNode *>& rvecStage/*, int search_cond*/ )
{
	if( GetState() == AVAILABLE )
	{
		rvecStage.push_back( this );
	}

	for( int i=0; i<m_NumNextStages; i++ )
		m_apNextStage[i]->GetAvailableStages_r( rvecStage ); 
}


void CStageGraph::Release()
{
	SafeDelete( m_pStageTree );
}


void CStageGraph::AddStageNode( const StageNodeDesc& desc )
{
	if( !m_pStageTree )
	{
		if( desc.base_node_name.length() == 0 )
            m_pStageTree = new StageNode( desc );
		else
			return;
	}

	// find base node
	StageNode* pNode = m_pStageTree->GetNode_r( desc.base_node_name );

	if( !pNode )
		return;

	// create a new node
	pNode->AddNode( desc );
}


void CStageGraph::MarkStageNodeAsCleared( const std::string& stage_node_name )
{
	if( !m_pStageTree )
		return;

	StageNode* pNode = m_pStageTree->GetNode_r( stage_node_name );

	if( !pNode )
		return;

	pNode->SetState( StageNode::CLEARED );
}


void CStageGraph::GetAvailableStages( std::vector<StageNode *>& rvecStage/*, int search_cond*/ )
{
	if( !m_pStageTree )
		return;

	m_pStageTree->GetAvailableStages_r( rvecStage );
}


PyObject* AddStageNode( PyObject* self, PyObject* args )
{
	char *base_node_name, *new_node_name, *script_filename;
	Vector3 pos = Vector3(0,0,0);

	int result = PyArg_ParseTuple( args, "sss|fff",
		&base_node_name, &new_node_name, &script_filename,
		&pos.x, &pos.y, &pos.z );

	StageNodeDesc desc;
	desc.base_node_name = base_node_name;
	desc.name = new_node_name;
	desc.script_filename = script_filename;

	StageGraph.AddStageNode( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* MarkStageNodeAsCleared( PyObject* self, PyObject* args )
{
	char *node_name;

	int result = PyArg_ParseTuple( args, "s", &node_name );

	StageGraph.MarkStageNodeAsCleared( node_name );

    Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef g_PyModuleStageGraphMethod[] =
{
	{ "AddStageNode",				AddStageNode,			METH_VARARGS, "add a stage node to the stage graph" },
	{ "MarkStageNodeAsCleared",		MarkStageNodeAsCleared,	METH_VARARGS, "mark a stage node as cleared" },
	{NULL, NULL}
};



} // namespace amorphous
