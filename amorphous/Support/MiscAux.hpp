#ifndef __MiscAux_H__
#define __MiscAux_H__


namespace amorphous
{

inline const char *GetBuildInfo()
{
#ifdef _MSC_VER
	#ifdef _DEBUG
		return "MSVC_Debug";
	#else
		return "MSVC_Release";
	#endif
#else /* MSC_VER */
	return "NonMSVC";
#endif /* MSC_VER */
}

} // namespace amorphous


#endif /* __MiscAux_H__ */
