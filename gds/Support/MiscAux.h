#ifndef __MiscAux_H__
#define __MiscAux_H__


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



#endif /* __MiscAux_H__ */
