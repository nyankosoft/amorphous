#ifndef  __TimeFormat_H__
#define  __TimeFormat_H__


namespace amorphous
{

class TimeFormat
{
public:
	enum Format
	{
		NONE,            ///< do not record time
		HHMMSSMS,        ///< hh:mm:ss.millisec	01:02:01.021 (default of CLogInputBase)
		HHMMSSFF,        ///< hh:mm:ss:ff	01:50:25 (f = frame = 1/60[sec])
		HHMMSS,          ///< hh:mm:ss		01:50:25	
		NUM_TIMEFORMATS
	};
};

} // namespace amorphous


#endif /* __TimeFormat_H__ */
