#include "PyModule_sound.hpp"

#include <Sound/SoundManager.hpp>
#include <Support/Log/DefaultLog.hpp>
#include <boost/python.hpp>
#include <iostream>


using namespace boost;
using namespace boost::python;

// changed function names to avoid overloading so that boost::python can see the difference

/// play From Resource
inline void PlayAtFR( const std::string& resource_path, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	CSoundHandle sound_handle( resource_path );
	SoundManager().PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}


inline void PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	SoundManager().PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}

/// play From Handle
inline void PlayFH( CSoundHandle& sound_handle )
{
	SoundManager().Play( sound_handle );
}

/// play From Resource
inline void PlayFR( const std::string& resource_path )
{
	CSoundHandle sound_handle( resource_path );
	SoundManager().Play( sound_handle );
}


inline void PlayFHD( CSoundHandle& sound_handle, const CSoundDesc& desc )
{
	SoundManager().Play( sound_handle, desc );
}


inline void PlayStream( const std::string& resource_path, double fadein_time, bool looped, int sound_group, U8 volume )
{
	SoundManager().PlayStream( resource_path, fadein_time, looped, sound_group, volume );
}

inline bool StopStream( const std::string& resource_path, double fadeout_time )
{
	return SoundManager().PlayStream( resource_path, fadeout_time );
}

inline CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
	                                                   const CSoundDesc& desc )
{
	return SoundManager().CreateSoundSource( sound_handle, desc );
}


inline CSoundSource *CreateSoundSourceFromResource( const std::string& resource_path,
	                                                   const CSoundDesc& desc )
{
	CSoundHandle sound_handle( resource_path );
	return SoundManager().CreateSoundSource( sound_handle, desc );
}


inline CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
		                                               CSoundSource::Type type,
											           CSoundSource::StreamType stream_type )
{
	CSoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == CSoundSource::Streamed );

	return SoundManager().CreateSoundSource( sound_handle, desc );
}


inline CSoundSource *CreateSoundSource( const std::string& resource_path,
		                                               CSoundSource::Type type,
											           CSoundSource::StreamType stream_type )
{
	CSoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == CSoundSource::Streamed );

	return SoundManager().CreateSoundSource( resource_path, desc );
}


inline void ReleaseSoundSource( CSoundSource*& pSoundSource )
{
	SoundManager().ReleaseSoundSource( pSoundSource );
}


inline void PauseAllSounds()
{
	SoundManager().PauseAllSounds();
}


inline void ResumeAllSounds()
{
	SoundManager().ResumeAllSounds();
}


inline void SetVolume( int volume_group, uint volume )
{
	SoundManager().SetVolume( volume_group, volume );
}


inline void SetMute( bool mute )
{
	SoundManager().SetMute( mute );
}


inline void SetListenerPosition( const Vector3& vPosition )
{
	SoundManager().SetListenerPosition( vPosition );
}

/*
inline void SetListenerPose( const Vector3& vPosition,
											const Vector3& vLookAtDirection,
		                                    const Vector3& vUp )
{
	SoundManager().SetListenerPose( vPosition, vLookAtDirection, vUp );
}
*/

inline void SetListenerPose( const Matrix34& pose )
{
	SoundManager().SetListenerPose(
		pose.vPosition,
		pose.matOrient.GetColumn(2),
		pose.matOrient.GetColumn(1)
		);
}


inline void SetListenerVelocity( const Vector3& vVelocity )
{
	SoundManager().SetListenerVelocity( vVelocity );
}


inline void CommitDeferredSettings()
{
	SoundManager().CommitDeferredSettings();
}



BOOST_PYTHON_MODULE(sound)
{
//	typedef CSoundManager SndMgr;

	class_<CSoundHandle>("SoundHandle")
		.def(init<const std::string&>())
//		.def("GetResourceName",   &CSoundHandle::GetResourceName) // error
		.def("SetResourceName",   &CSoundHandle::SetResourceName)
	;

	class_<CSoundDesc>("SoundDesc")
		.def_readwrite("Position",           &CSoundDesc::Position)
		.def_readwrite("Direction",          &CSoundDesc::Direction)
		.def_readwrite("Velocity",           &CSoundDesc::Velocity)
		.def_readwrite("Streamed",           &CSoundDesc::Streamed)
		.def_readwrite("Loop",               &CSoundDesc::Loop)
		.def_readwrite("Volume",             &CSoundDesc::Volume)
		.def_readwrite("MaxDistance",        &CSoundDesc::MaxDistance)
		.def_readwrite("ReferenceDistance",  &CSoundDesc::ReferenceDistance)
		.def_readwrite("RollOffFactor",      &CSoundDesc::RollOffFactor)
		.def_readwrite("SoundSourceType",    &CSoundDesc::SoundSourceType)
//		.def_readwrite("SourceManagement",   &CSoundDesc::SourceManagement) // Should always be auto?
		.def_readwrite("SoundGroup",         &CSoundDesc::SoundGroup)
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
	class_< CSoundManagerWrapper//, boost::shared_ptr<> >("SoundManager")
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
