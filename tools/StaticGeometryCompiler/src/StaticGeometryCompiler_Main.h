#include <string>

class CLogOutput_ScrolledTextBuffer;

extern std::string m_OutputFilepath;
extern CLogOutput_ScrolledTextBuffer *g_pLogOutput;

extern bool CompileStaticGeometry( const std::string& filename );
extern bool RunStaticGeometryCompiler( const std::string& cmd_line, const std::string& initial_working_directory );
