
#ifndef  __TEXTFILESCANNER_H__
#define  __TEXTFILESCANNER_H__


#include <stdio.h>

#include <string>

#include "3DMath/Vector3.h"


class CTextFileScanner
{
	enum param { MAX_TAG_LENGTH = 128, MAX_LINE_LENGTH = 2048 };

	FILE* m_fp;

	/// buffer for current line
	char m_acCurrentLine[MAX_LINE_LENGTH];

	/// buffer to temporarily hold tag
	char m_acTag[MAX_TAG_LENGTH];

	std::string m_strTag;

	void Init();

	void UpdateTagStr() { sscanf( m_acCurrentLine, "%s", m_acTag ); m_strTag = m_acTag; }

	bool GetBoolTagStr( std::string& tag_true, std::string& tag_false, const std::string& bool_tag_str );

public:

	CTextFileScanner() { Init(); }

	CTextFileScanner( const std::string& strFilename ) { Init(); OpenFile(strFilename); }
	
	~CTextFileScanner() { CloseFile(); }

	bool OpenFile( const std::string& strFilename );

	void CloseFile() { if( m_fp ) fclose(m_fp); m_fp = NULL; }

	/// returns true if the file is open and ready to scan
	bool IsReady() const { return (m_fp != NULL); }

	/// move to the next line
	/// returns false if the scanner reached the end of the file
	bool NextLine();

	/// returns if the scanning has reached the end of the file
	bool End();

	/// get the current entire line
	inline void GetCurrentLine( std::string& str ) { str = m_acCurrentLine; }

	inline const std::string GetCurrentLine() const { std::string str = m_acCurrentLine; return str; }

	/// get the first std::string component of the current line
	bool GetTagString( std::string& strTag );

	const std::string& GetTagString() const { return m_strTag; }

	const char *GetTagStr() const { return m_acTag; }

	/// scan the current line in various formats

	void ScanLine( std::string& strTag, std::string& str1 );
	void ScanLine( std::string& strTag, std::string& str1, std::string& str2 );

	///  value is loaded as a float from the text file
	void ScanLine( std::string& strTag, double& f );

	void ScanLine( std::string& strTag, float& f );
	void ScanLine( std::string& strTag, float& f1, float& f2 );
	void ScanLine( std::string& strTag, float& f1, float& f2, float& f3 );
	void ScanLine( std::string& strTag, float& f1, float& f2, float& f3, float& f4 );

	void ScanLine( std::string& strTag, std::string& strEntry, float& f );
	void ScanLine( std::string& strTag, std::string& strEntry, float& f1, float& f2 );
	void ScanLine( std::string& strTag, std::string& strEntry, float& f1, float& f2, float& f3 );
	void ScanLine( std::string& strTag, std::string& strEntry, float& f1, float& f2, float& f3, float& f4 );

	void ScanLine( std::string& strTag, int &i );
	void ScanLine( std::string& strTag, int &i1, int &i2 );
	void ScanLine( std::string& strTag, int &i1, int &i2, int &i3 );
	void ScanLine( std::string& strTag, int &i1, int &i2, int &i3, int &i4 );
	void ScanLine( std::string& strTag, int &i1, int &i2, int &i3, int &i4, int &i5 );

	void ScanLine( std::string& strTag, Vector3& v );
	void ScanLine( std::string& strTag, Vector3& v1, Vector3& v2 );
	void ScanLine( std::string& strTag, int& i, Vector3& v );
	void ScanLine( std::string& strTag, std::string& str, Vector3& v );
	void ScanLine( std::string& strTag, std::string& str, Vector3& v1, Vector3& v2 );

	void ScanLine( std::string& strTag, int &i, std::string& strEntry );
	void ScanLine( std::string& strTag, float &f, std::string& strEntry );

	void ScanLine( std::string& strTag, std::string& strEntry, int &i );
	void ScanLine( std::string& strTag, std::string& strEntry, int &i0, int &i1 );

	bool ScanBool( std::string& strTag, const std::string& bool_str, bool& b );

	bool ScanSentence( std::string& strTag, std::string& text );


	/// loads the params and returns true if first arg 'tag' matches the tag on the current line
	/// otherwise just returns false
	bool TryScanLine( const char *tag, std::string& str1 );
	bool TryScanLine( const char *tag, std::string& str1, std::string& str2 );

	///  value is loaded as a float from the text file
	bool TryScanLine( const char *tag, double& f );

	bool TryScanLine( const char *tag, float& f );
	bool TryScanLine( const char *tag, float& f1, float& f2 );
	bool TryScanLine( const char *tag, float& f1, float& f2, float& f3 );
	bool TryScanLine( const char *tag, float& f1, float& f2, float& f3, float& f4 );

	bool TryScanLine( const char *tag, std::string& strEntry, float& f );
	bool TryScanLine( const char *tag, std::string& strEntry, float& f1, float& f2 );
	bool TryScanLine( const char *tag, std::string& strEntry, float& f1, float& f2, float& f3 );
	bool TryScanLine( const char *tag, std::string& strEntry, float& f1, float& f2, float& f3, float& f4 );

	bool TryScanLine( const char *tag, int &i );
	bool TryScanLine( const char *tag, int &i1, int &i2 );
	bool TryScanLine( const char *tag, int &i1, int &i2, int &i3 );
	bool TryScanLine( const char *tag, int &i1, int &i2, int &i3, int &i4 );
	bool TryScanLine( const char *tag, int &i1, int &i2, int &i3, int &i4, int &i5 );

	bool TryScanLine( const char *tag, Vector3& v );
	bool TryScanLine( const char *tag, Vector3& v1, Vector3& v2 );
	bool TryScanLine( const char *tag, int& i, Vector3& v );
	bool TryScanLine( const char *tag, std::string& str, Vector3& v );
	bool TryScanLine( const char *tag, std::string& str, Vector3& v1, Vector3& v2 );

	bool TryScanLine( const char *tag, int &i, std::string& strEntry );
	bool TryScanLine( const char *tag, float &f, std::string& strEntry );

	bool TryScanLine( const char *tag, std::string& str, int &i );
	bool TryScanLine( const char *tag, std::string& str, int &i0, int &i1 );

	bool TryScanBool( const char *tag, const std::string& bool_str, bool& b );

	bool TryScanSentence( const char *tag, std::string& text );

	/// searchs a line that begins with the specified tag.
	/// The search will stop at the first hit, and the subsequenet ScanLine()
	/// or TryScanLine() call will be applied to the line which contains the tag.
	/// The file pointer position is reset when this method is called.
	bool FindLineWithTag( const std::string& tag );
};



#endif		/*  __TEXTFILESCANNER_H__  */