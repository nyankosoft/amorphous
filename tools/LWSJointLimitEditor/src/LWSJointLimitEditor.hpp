#ifndef __LWSJointLimitEditor_HPP__
#define __LWSJointLimitEditor_HPP__


#include <string>
#include <vector>
#include <list>
#include <map>


template<typename T>
class ranged_pair
{
public:
	T min, max;

	void set( const T& _min, const T& _max )
	{
		min = _min;
		max = _max;
	}
};

typedef std::list<std::string>::iterator list_str_iterator;

class BoneItemInfo
{
public:
	std::string m_Name;
	list_str_iterator m_StartLine;

	list_str_iterator m_HeadingLimitLine;
	list_str_iterator m_PitchLimitLine;
	list_str_iterator m_BankLimitLine;

	BoneItemInfo( int start_line = 0 )
//		:
//	m_HeadingLimitLine(-1),
//	m_PitchLimitLine(-1),
//	m_BankLimitLine(-1),
//	m_StartLine(start_line)
	{}
};


class JointLimit
{
public:

	enum Flags
	{
		HEADING = (1 << 0),
		PITCH   = (1 << 1),
		BANK    = (1 << 2),
	};

	// Stores on which rotation angles the limits are specified
	unsigned long m_Flags;

	ranged_pair<float> heading;
	ranged_pair<float> pitch;
	ranged_pair<float> bank;

	JointLimit()
		:
	m_Flags(0)
	{
		heading.set( 0, 0 );
		pitch.set( 0, 0 );
		bank.set( 0, 0 );
	}
};



class LWSJointLimitEditor
{
//	std::vector<JointLimit> m_JointLimits;
	std::map<std::string,JointLimit> m_JointNamesToJointLimits;

//	std::vector<std::string> m_LWSFileContent;
	std::list<std::string> m_LWSFileContent;

	std::vector<BoneItemInfo> m_BoneItems;
	std::map<std::string,BoneItemInfo> m_BoneNameToBoneItem;

	std::string m_LWSFilepath;

	void InsertJointLimitLines( FILE *fp, const std::string& joint_name, const std::string *limit_lines );

public:

	LWSJointLimitEditor() {}

	/// Returns 0 on success
	int LoadSceneFile( const std::string& lws_filepath );

	/// Returns 0 on success
	int LoadCSVJointLimitTable( const std::string& csv_filepath );

	int Write(/*  const string& csv_filepath */);

	int SetLimits();
};



#endif  /*  __LWSJointLimitEditor_HPP__  */
