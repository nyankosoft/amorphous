#include "AABTree.hpp"

#include "Support/StringAux.hpp"

using namespace std;


int CAABNode::ms_DefaultReserveSize = 1024;


static inline void print_tabs( FILE *fp, int depth )
{
	for( int i=0; i<depth; i++ )
		fprintf( fp, "\t" );
}


static inline void print_spaces( FILE *fp, int depth )
{
	for( int i=0; i<depth; i++ )
		fprintf( fp, " " );
}


void  WriteNodeToFile_r( int node_index, vector<CAABNode>& nodes, int depth, FILE *fp )
{
	const CAABNode& node = nodes[node_index];

	print_spaces(fp,depth*2);
	fprintf( fp, "[%d]----------------------\n", node_index );

	print_spaces(fp,depth*2);
	fprintf( fp, "depth: %d, %s\n", depth, to_string(node.aabb).c_str() );

	print_spaces(fp,depth*2);
	fprintf( fp, "geoms(%d): ", (int)node.veciGeometryIndex.size() );
	for( size_t i=0; i<node.veciGeometryIndex.size(); i++ )
	{
		fprintf( fp, "%d, ", node.veciGeometryIndex[i] );
	}
	fprintf( fp, "\n" );

	for( int i=0; i<2; i++ )
	{
		if( 0 <= node.child[i] )
			WriteNodeToFile_r( node.child[i], nodes, depth + 1, fp );
	}
}
