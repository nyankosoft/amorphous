#include "PyModule_sound.hpp"
#include "../Sound/SoundManager.hpp"
#include "../Support/Log/DefaultLog.hpp"
#include <boost/python.hpp>


namespace amorphous
{


using namespace boost;
using namespace boost::python;

// changed function names to avoid overloading so that boost::python can see the difference

/// play From Resource
inline void PlayAtFR( const std::string& resource_path, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	SoundHandle sound_handle( resource_path );
	GetSoundManager().PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}


inline void PlayAt( SoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	GetSoundManager().PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}

/// play From Handle
inline void PlayFH( SoundHandle& sound_handle )
{
	GetSoundManager().Play( sound_handle );
}

/// play From Resource
inline void PlayFR( const std::string& resource_path )
{
	SoundHandle sound_handle( resource_path );
	GetSoundManager().Play( sound_handle );
}


inline void PlayFHD( SoundHandle& sound_handle, const SoundDesc& desc )
{
	GetSoundManager().Play( sound_handle, desc );
}


inline void PlayStream( const std::string& resource_path, double fadein_time, bool looped, int sound_group, U8 volume )
{
	GetSoundManager().PlayStream( resource_path, fadein_time, looped, sound_group, volume );
}

inline bool StopStream( const std::string& resource_path, double fadeout_time )
{
	return GetSoundManager().PlayStream( resource_path, fadeout_time );
}

inline SoundSource *CreateSoundSource( SoundHandle& sound_handle,
	                                                   const SoundDesc& desc )
{
	return GetSoundManager().CreateSoundSource( sound_handle, desc );
}


inline SoundSource *CreateSoundSourceFromResource( const std::string& resource_path,
	                                                   const SoundDesc& desc )
{
	SoundHandle sound_handle( resource_path );
	return GetSoundManager().CreateSoundSource( sound_handle, desc );
}


inline SoundSource *CreateSoundSource( SoundHandle& sound_handle,
		                                               SoundSource::Type type,
											           SoundSource::StreamType stream_type )
{
	SoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == SoundSource::Streamed );

	return GetSoundManager().CreateSoundSource( sound_handle, desc );
}


inline SoundSource *CreateSoundSource( const std::string& resource_path,
		                                               SoundSource::Type type,
											           SoundSource::StreamType stream_type )
{
	SoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == SoundSource::Streamed );

	return GetSoundManager().CreateSoundSource( resource_path, desc );
}


inline void ReleaseSoundSource( SoundSource*& pSoundSource )
{
	GetSoundManager().ReleaseSoundSource( pSoundSource );
}


inline void PauseAllSounds()
{
	GetSoundManager().PauseAllSounds();
}


inline void ResumeAllSounds()
{
	GetSoundManager().ResumeAllSounds();
}


inline void SetVolume( int volume_group, uint volume )
{
	GetSoundManager().SetVolume( volume_group, volume );
}


inline void SetMute( bool mute )
{
	GetSoundManager().SetMute( mute );
}


inline void SetListenerPosition( const Vector3& vPosition )
{
	GetSoundManager().SetListenerPosition( vPosition );
}

/*
inline void SetListenerPose( const Vector3& vPosition,
											const Vector3& vLookAtDirection,
		                                    const Vector3& vUp )
{
	GetSoundManager().SetListenerPose( vPosition, vLookAtDirection, vUp );
}
*/

inline void SetListenerPose( const Matrix34& pose )
{
	GetSoundManager().SetListenerPose(
		pose.vPosition,
		pose.matOrient.GetColumn(2),
		pose.matOrient.GetColumn(1)
		);
}


inline void SetListenerVelocity( const Vector3& vVelocity )
{
	GetSoundManager().SetListenerVelocity( vVelocity );
}


inline void CommitDeferredSettings()
{
	GetSoundManager().CommitDeferredSettings();
}



BOOST_PYTHON_MODULE(sound)
{
//	typedef SoundManager SndMgr;

	class_<SoundHandle>("SoundHandle")
		.def(init<const std::string&>())
//		.def("GetResourceName",   &SoundHandle::GetResourceName) // error
		.def("SetResourceName",   &SoundHandle::SetResourceName)
	;

	class_<SoundDesc>("SoundDesc")
		.def_readwrite("Position",           &SoundDesc::Position)
		.def_readwrite("Direction",          &SoundDesc::Direction)
		.def_readwrite("Velocity",           &SoundDesc::Velocity)
		.def_readwrite("Streamed",           &SoundDesc::Streamed)
		.def_readwrite("Loop",               &SoundDesc::Loop)
		.def_readwrite("Volume",             &SoundDesc::Volume)
		.def_readwrite("MaxDistance",        &SoundDesc::MaxDistance)
		.def_readwrite("ReferenceDistance",  &SoundDesc::ReferenceDistance)
		.def_readwrite("RollOffFactor",      &SoundDesc::RollOffFactor)
		.def_readwrite("SoundSourceType",    &SoundDesc::SoundSourceType)
//		.def_readwrite("SourceManagement",   &SoundDesc::SourceManagement) // Should always be auto?
		.def_readwrite("SoundGroup",         &SoundDesc::SoundGroup)
	;

	def( "PlayAt",              PlayAt,   ( python::arg("sound_handle"),  python::arg("pos"), python::arg("max_dist") = 1000.0f, python::arg("ref_dist") = 100.0f, python::arg("rolloff_factor") = 1.0f ) );
	def( "PlayAt",              PlayAtFR, ( python::arg("resource_path"), python::arg("pos"), python::arg("max_dist") = 1000.0f, python::arg("max_dist") = 100.0f, python::arg("rolloff_factor") = 1.0f ) );
	def( "Play",                PlayFH,   ( python::arg("sound_handle") ) );
	def( "Play",                PlayFR,   ( python::arg("resource_path") ) );
	def( "Play",                PlayFHD,  ( python::arg("sound_handle"), python::arg("desc") ) );
	def( "PlayStream",          PlayStream, ( python::arg("resource_path"), python::arg("fadein_time") = 0, python::arg("looped") = 0, python::arg("sound_group") = 0, python::arg("volume") = 0xFF ) );
	def( "StopStream",          StopStream, ( python::arg("resource_path"), python::arg("fadeout_time") = 0 ) );
	def( "SetListenerPosition", SetListenerPosition, ( python::arg("pos") ) );
	def( "SetListenerPose",     SetListenerPose,     ( python::arg("pose") ) );
	def( "SetListenerVelocity", SetListenerVelocity, ( python::arg("vel") ) );
/*	def( "SetMute",             SetMute );
*/

/*
	class_< SoundManagerWrapper//, boost::shared_ptr<> >("SoundManager")
		.def( "PlayAt",              &SndMgr::PlayAt, ( python::arg("sound_handle"),  python::arg("pos"), python::arg("max_dist") = 1000.0f, python::arg("ref_dist") = 100.0f, python::arg("rolloff_factor") = 1.0f ) )
		.def( "PlayAt",              &SndMgr::PlayAt, ( python::arg("resource_path"), python::arg("pos"), python::arg("max_dist") = 1000.0f, python::arg("max_dist") = 100.0f, python::arg("rolloff_factor") = 1.0f ) )
		.def( "Play",                &SndMgr::Play,   ( python::arg("sound_handle") ) )
		.def( "Play",                &SndMgr::Play,   ( python::arg("resource_path") ) )
		.def( "Play",                &SndMgr::Play,   ( python::arg("sound_handle"), python::arg("desc") ) )
		.def( "PlayStream",          &SndMgr::PlayStream, ( python::arg("resource_path"), python::arg("fill_color_0"), python::arg("corner_radius"), python::arg("layer") = 0 ) )
		.def( "StopStream",          &SndMgr::StopStream, ( python::arg("resource_path"), python::arg("frame_color_0"), python::arg("corner_radius"), python::arg("frame_width"), python::arg("layer") = 0 ) )
		.def( "SetListenerPosition", &SndMgr::SetListenerPosition, ( python::arg("pos") ) )
		.def( "SetListenerPose",     &SndMgr::SetListenerPose, ( python::arg("pose") ) )
		.def( "SetListenerVelocity", &SndMgr::SetListenerVelocity, ( python::arg("vel") ) )
		.def( "SetMute",             &SndMgr::SetMute )
	;
*/
}


void RegisterPythonModule_sound()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("sound", initsound) == -1)
	{
		const char *msg = "Failed to add 'sound' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}


} // namespace amorphous
