#ifndef  __GM_Loader_H__
#define  __GM_Loader_H__


#include <vector>
#include <string>



class CGM_Loader
{

	VarType01 m_Variable01;
	VarType02 m_Variable02;


public:

	enum eType
	{
		TAG0 = 0,
		TAG1,
		TAG2,
		TAG3
	};


	CGM_Loader();
	~CGM_Loader();

	void LoadFromFile( const std::string& filename );

	void LoadDesc( CTextFileScanner& scanner );

	void MemberFunction03();

	inline VarType01 GetVariable01() const { return m_Variable01; }
	inline VarType02 GetVariable02() const { return m_Variable02; }

	inline void SetVariable01( const VarType01& obj ) const { m_Variable01 = obj; }
	inline void SetVariable02( const VarType02& obj ) const { m_Variable02 = obj; }

};



#endif		/*  __GM_Loader_H__  */
