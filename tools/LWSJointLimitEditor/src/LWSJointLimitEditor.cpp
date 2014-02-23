#include "LWSJointLimitEditor.hpp"
#include "amorphous/Support/StringAux.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

typedef boost::tokenizer< boost::escaped_list_separator<char> > csv_tokenizer;



class RotationAxis
{
public:
	enum Name
	{
		HEADING = 0,
		PITCH,
		BANK,
		NUM_ROTATION_AXES
	};
};


/*
bone name, min heading, max heading, min pitch, max pitch, min bank, max bank
neck,-,-,-,-,-,-
spine1,-,-,-,-,-,-
spine2,-,-,-,-,-,-
spine3,-,-,-,-,-,-
r-leg,-,-,-,-,-,-
l-leg,-,-,-,-,-,-
r-arm,-,-,-,-,-,-
l-arm,-,-,-,-,-,-
r-shoulder,-,-,-,-,-,-
l-shoulder,-,-,-,-,-,-
r-thumb-root
r-thumb-base
r-thumb-mid
r-thumb-tip
r-index_finger-root
r-index_finger-base
r-index_finger-mid
r-index_finger-tip
r-middle_finger-root
r-middle_finger-base
r-middle_finger-mid
r-middle_finger-tip
r-ring_finger-root
r-ring_finger-base
r-ring_finger-mid
r-ring_finger-tip
r-pinky-root
r-pinky-base
r-pinky-mid
r-pinky-tip
l-thumb-root
l-thumb-base
l-thumb-mid
l-thumb-tip
l-index_finger-root
l-index_finger-base
l-index_finger-mid
l-index_finger-tip
l-middle_finger-root
l-middle_finger-base
l-middle_finger-mid
l-middle_finger-tip
l-ring_finger-root
l-ring_finger-base
l-ring_finger-mid
l-ring_finger-tip
l-pinky-root
l-pinky-base
l-pinky-mid
l-pinky-tip

*/


template<class string_buffer>
int load_text_file( const string& filepath, string_buffer& dest_buffer )
{
	FILE *fp = fopen( filepath.c_str(), "r" );
	if( !fp )
		return -1;

	char line_buffer[1024];
	memset( line_buffer, 0, sizeof(line_buffer) );
	while(1)
	{
		char *read = fgets( line_buffer, sizeof(line_buffer) / sizeof(char) - 1, fp );
		if( !read )
			break;

		dest_buffer.push_back( line_buffer );
	}

	fclose( fp );
	return 0;
}


int load_text_file( const string& filepath, std::vector<std::string>& dest_buffer )
{
	FILE *fp = fopen( filepath.c_str(), "r" );
	if( !fp )
		return -1;

	char line_buffer[1024];
	memset( line_buffer, 0, sizeof(line_buffer) );
	while(1)
	{
		char *read = fgets( line_buffer, sizeof(line_buffer) / sizeof(char) - 1, fp );
		if( !read )
			break;

		dest_buffer.push_back( line_buffer );
	}

	fclose( fp );
	return 0;
}


int LWSJointLimitEditor::LoadSceneFile( const string& lws_filepath )
{
	m_LWSFileContent.resize( 0 );

	m_BoneItems.resize( 0 );
	m_BoneNameToBoneItem.clear();

//	InitBoneItems();

	if( lws_filepath.length() < 5 )
		return -1;

	m_LWSFilepath = lws_filepath;

	int ret = load_text_file( lws_filepath, m_LWSFileContent );
	if( ret != 0 )
		return ret;

//	char tag_buffer[1024];
	char name_buffer[1024], next_tag_buffer[64];
	vector<string> tokens;
	list<string>::iterator itr;
	for( itr = m_LWSFileContent.begin();
		 itr != m_LWSFileContent.end();
		 itr++ )
	{
		const string& line_buffer = *itr;

//		memset( tag_buffer, 0, sizeof(tag_buffer) );
//		sscanf( tag_buffer, "%s", line_buffer.c_str() );
//		string tag = tag_buffer;

		tokens.resize(0);
		SeparateStrings( tokens, line_buffer.c_str(), " \t\n" );

		if( tokens.empty() )
			continue;

		string tag(tokens[0]);

		if( tag == "AddBone" )
		{
			m_BoneItems.push_back( BoneItemInfo() );
			m_BoneItems.back().m_HeadingLimitLine = m_LWSFileContent.end();
			m_BoneItems.back().m_PitchLimitLine   = m_LWSFileContent.end();
			m_BoneItems.back().m_BankLimitLine    = m_LWSFileContent.end();

			list<string>::iterator next_itr = itr;
			next_itr++;

			// get the bone name - assumes that they are always the next line of "AddBone"
			sscanf( (*next_itr).c_str(), "%s %s", next_tag_buffer, name_buffer );
			m_BoneItems.back().m_Name = name_buffer;
			if( name_buffer
			 && 0 < strlen(name_buffer)
			 && name_buffer[strlen(name_buffer)-1] == '\n' )
			{
				name_buffer[strlen(name_buffer)-1] = '\0';
			}
		}
		else if( tag == "HLimits" )
		{
			if( !m_BoneItems.empty() )
				m_BoneItems.back().m_HeadingLimitLine = itr;
		}
		else if( tag == "PLimits" )
		{
			if( !m_BoneItems.empty() )
				m_BoneItems.back().m_PitchLimitLine = itr;
		}
		else if( tag == "BLimits" )
		{
			if( !m_BoneItems.empty() )
				m_BoneItems.back().m_BankLimitLine = itr;
		}
	}

	// copy vector of bone items to map of bone names and bone items
	for( size_t i=0; i<m_BoneItems.size(); i++ )
	{
		m_BoneNameToBoneItem[m_BoneItems[i].m_Name] = m_BoneItems[i];
	}

	return 0;
}


inline void process_limit_string( int column_index, const std::string& src, JointLimit& dest )
{
	if( src == "-" )
		return;

	float val = to_float( src );

	switch( column_index )
	{
	case 1: dest.m_Flags |= JointLimit::HEADING; dest.heading.min = val; break;
	case 2: dest.m_Flags |= JointLimit::HEADING; dest.heading.max = val; break;
	case 3: dest.m_Flags |= JointLimit::PITCH;   dest.pitch.min   = val; break;
	case 4: dest.m_Flags |= JointLimit::PITCH;   dest.pitch.max   = val; break;
	case 5: dest.m_Flags |= JointLimit::BANK;    dest.bank.min    = val; break;
	case 6: dest.m_Flags |= JointLimit::BANK;    dest.bank.max    = val; break;
	default:
		break;
	}
}


int LWSJointLimitEditor::LoadCSVJointLimitTable( const string& csv_filepath )
{
	vector<string> file_buffer;
	file_buffer.reserve( 128 ); // set this to a value which is larger than the total number of bones to avoid repeated memory allocaitons
	int ret = load_text_file( csv_filepath, file_buffer );

	for( int i=0; i<(int)file_buffer.size(); i++ )//const std::string& line_buffer, file_buffer )
	{
		const string& line_buffer = file_buffer[i];

		if( i == 0 )
			continue;

		JointLimit limits;
		string bone_name;

		csv_tokenizer tok( line_buffer );
		int column = 0;
		for( csv_tokenizer::iterator beg=tok.begin();
			 beg!=tok.end() && column < 7; // bone name, H.min, H.max, P.min, P.max, B.min, B.max
			 ++beg, ++column )
		{
			string item = *beg;
			if( 0 < item.length() && item.substr( item.length() - 1 ) == "\n" )
				item = item.substr( 0, item.length() - 1 );

			switch(column)
			{
			case 0:
				bone_name = item;
				break;

			default:
				process_limit_string( column, item, limits );
				break;
			}
		}

		m_JointNamesToJointLimits[bone_name] = limits;
	}

	return 0;
}


void LWSJointLimitEditor::InsertJointLimitLines( FILE *fp,
												const std::string& joint_name,
												const std::string *limit_lines
												)
{
	map<string,JointLimit>::iterator itr = m_JointNamesToJointLimits.find( joint_name );
	if( itr == m_JointNamesToJointLimits.end() )
		return;

	const JointLimit& limits = itr->second;

	if( limits.m_Flags & JointLimit::HEADING )
		fprintf( fp, "HLimits %.3f %.3f\n", limits.heading.min, limits.heading.max ); // write the limit specified in the limit list
	else if( 0 < limit_lines[0].length() )
		fprintf( fp, "%s", limit_lines[0].c_str() ); // resotre the original limits

	if( limits.m_Flags & JointLimit::PITCH )
		fprintf( fp, "PLimits %.3f %.3f\n", limits.pitch.min,   limits.pitch.max );
	else if( 0 < limit_lines[0].length() )
		fprintf( fp, "%s", limit_lines[1].c_str() );

	if( limits.m_Flags & JointLimit::BANK )
		fprintf( fp, "BLimits %.3f %.3f\n", limits.bank.min,    limits.bank.max );
	else if( 0 < limit_lines[0].length() )
		fprintf( fp, "%s", limit_lines[2].c_str() );

}


int LWSJointLimitEditor::SetLimits()
{
	map<string,JointLimit>::iterator itr;// m_JointNamesToJointLimits
	for( itr = m_JointNamesToJointLimits.begin();
		 itr != m_JointNamesToJointLimits.end();
		 itr++ )
	{
		map<string,BoneItemInfo>::iterator itrBoneItem = m_BoneNameToBoneItem.find( itr->first );

		if( itrBoneItem == m_BoneNameToBoneItem.end() )
			continue;

		const BoneItemInfo& bone_item = itrBoneItem->second;
		*(bone_item.m_HeadingLimitLine) = fmt_string( "HLimits %f %f", 0.0f, 0.0f );
		*(bone_item.m_PitchLimitLine)   = string();
		*(bone_item.m_BankLimitLine)    = string();
	}

	return 0;
}


int LWSJointLimitEditor::Write(/*  const string& csv_filepath */)
{
	using namespace boost::filesystem;

	if( m_LWSFilepath.length() < 5 )
		return -1;

	if( m_LWSFileContent.empty() )
		return -1;

	// Create a backup of the original LW scene file.
	string bak_filepath( m_LWSFilepath + ".bak" );
	if( exists( bak_filepath ) )
		remove( bak_filepath );
	copy_file( m_LWSFilepath, bak_filepath );

	FILE *fp = fopen( m_LWSFilepath.c_str(), "w" );
	if( !fp )
		return -1;

	int current_bone_item_index = -1;
	const int num_orig_lines = (int)m_LWSFileContent.size();
	vector<string> tokens;
	string current_bone_name;
	string limit_lines[RotationAxis::NUM_ROTATION_AXES];
	list<string>::iterator itr;
	for( itr = m_LWSFileContent.begin();
		 itr != m_LWSFileContent.end();
		 itr++ )
	{
		string& line_buffer = *itr;

		tokens.resize(0);

		SeparateStrings( tokens, line_buffer.c_str(), " \t\n" );

		const string tag = tokens.empty() ? "" : tokens[0];

		if( tag == "PathAlignLookAhead" )
		{
			InsertJointLimitLines( fp, current_bone_name, limit_lines );
		}
		else if( tag == "BoneName" )
		{
			for( int j=0; j<3; j++ )
			{
				limit_lines[j] = "";
			}

			current_bone_name = tokens[1];
		}
		else if( tag == "HLimits" )
		{
			limit_lines[0] = line_buffer;
		}
		else if( tag == "PLimits" )
		{
			limit_lines[1] = line_buffer;
		}
		else if( tag == "BLimits" )
		{
			limit_lines[2] = line_buffer;
		}

		if( tag != "HLimits"
		 && tag != "PLimits"
		 && tag != "BLimits" )
		{
			// All the limits are written in InsertJointLimitLines()
			fprintf( fp, "%s", line_buffer.c_str() );
		}
	}

	fclose( fp );

	return 0;
}
