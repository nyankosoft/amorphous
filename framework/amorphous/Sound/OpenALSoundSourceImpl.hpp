#ifndef  __OpenALSoundSourceImpl_H__
#define  __OpenALSoundSourceImpl_H__


#include "fwd.hpp"
#include "SoundSource.hpp"
#include "amorphous/Support/SerializableStream.hpp"
#include "amorphous/Support/prealloc_pool.hpp"
#include "amorphous/Support/Macro.h"
#include <memory>
#include <thread>
#include <al.h>
#include <alut.h>
#include <vorbis/vorbisfile.h>


namespace amorphous
{


extern bool GetFormatFrequencyChannelsBufferSize( OggVorbis_File& ogg_vorbis_file,
										          ulong& ulFormat,
										          ulong& ulFrequency,
										          ulong& ulChannels,
										          ulong& ulBufferSize );


#define LOG_PRINT_AL_ERROR() { ALenum ret = alGetError();	if( ret != AL_NO_ERROR ) { LOG_PRINT_ERROR( GET_TEXT_FROM_ID( ret, g_OpenALErrors ) ); } }


#define ID_AND_TEXT( id ) IDAndTextPair( id, #id )


class IDAndTextPair
{
public:
	const int id;
	const char *text;

	IDAndTextPair( int _id, const char *_text )
		:
	id(_id),
	text(_text)
	{}
};

extern const IDAndTextPair g_OpenALErrors[6];


#define GET_TEXT_FROM_ID( id, id_and_text_array ) GetTextFromID( id, id_and_text_array, numof(id_and_text_array) )


inline const char* GetTextFromID( int id, const IDAndTextPair *id_and_text_array, int num_elements )
{
	for( int i=0; i<num_elements; i++ )
	{
		if( id == id_and_text_array[i].id )
			return id_and_text_array[i].text;
	}

	return "";
}


class COpenALSoundSourceImpl : public SoundSourceImpl
{
	/// set by OpenALSoundManagerImpl
	SoundSource::Management m_ManagementType;

	SoundSource::Type m_SourceType;

	pooled_object_handle m_BufferHandle;

	bool m_Released;

	/// used by prealloc_pool
	int m_StockIndex;
	int m_StockID;

protected:

	OpenALSoundManagerImpl *m_pManager;

	ALuint m_uiSource;

	/// used by the stream sound source
	std::string m_ResourcePath;

public:

	COpenALSoundSourceImpl();

	virtual ~COpenALSoundSourceImpl();

	void SetPosition( const Vector3& vPosition );

	void SetVelocity( const Vector3& vVelocity );

	void SetPose( const Matrix34& pose );

	bool IsDone();

	void SetResourcePath( const std::string& resource_path ) { m_ResourcePath = resource_path; }

	SoundSource::Type GetSoundType() { return m_SourceType; }

	SoundSource::Management GetManagementType() { return m_ManagementType; }

	virtual void OnCreated() { m_Released = false; }

	virtual void OnReleased();

	SoundSource::State GetState();

//	void GetTextInfo( std::string& dest_buffer );

	void CreateSource();

	void ReleaseSource();

	/// implemented by the streamed sound source
	virtual void StartStreamThread() {}

	// functions used by prealloc_pool

	int GetStockIndex() const { return m_StockIndex; }
	void SetStockIndex( int index ) { m_StockIndex = index; }

	int GetStockID() const { return m_StockID; }
	void SetStockID( int id ) { m_StockID = id; }

	friend class OpenALSoundManagerImpl;
};


/// owns sound buffers
/// - owns a thread?
class COpenALStreamedSoundSourceImpl : public COpenALSoundSourceImpl
{
	enum Params
	{
		NUM_DEFAULT_BUFFERS_FOR_STREAMING = 4,
		NUM_MAX_BUFFERS_FOR_STREAMING     = 8,
		DEFAULT_SERVICE_UPDATE_PERIOD_MS  = 20
	};

	enum StreamMethod
	{
		StreamFromDisk,   ///< stream directly from disk
		StreamFromMemory, ///< load on memory and the stream from that memory
		NumStreamMethods
	};

	bool m_Loop;

	StreamMethod m_StreamMethod;


	ALuint m_uiBuffers[NUM_MAX_BUFFERS_FOR_STREAMING];


	int m_NumBuffersForStreaming;

	/// How long the thread sleeps 
	int	m_ServiceUpdatePeriodMS;

	boost::mutex m_StreamSoundMutex;

	std::shared_ptr<boost::thread> m_pThread;

	/// stores an encoded data loaded from disk
	/// - Not used when the sound is streamed directly from the disk
	stream_buffer m_Buffer;

	int m_NumTotalBuffersProcessed;

	SoundSource::State m_RequestedState;

	bool m_ExitStreamThread;

private:

	bool OpenOrLoadOggResource( const std::string& resource_path,
								OggVorbis_File& sOggVorbisFile,
								SerializableStream& src_buffer );

	inline SoundSource::State GetRequestedState();

	int PlayStream();

	void EndStreamThread();

public:

	COpenALStreamedSoundSourceImpl();

	~COpenALStreamedSoundSourceImpl();

	void Release();

	void Play( double fadein_time );

	void Stop( double fadeout_time );

	void Pause( double fadeout_time );

	void Resume( double fadein_time );

	void SetLoop( bool loop );

	SoundSource::StreamType GetStreamType() { return SoundSource::Streamed; }

	void GetTextInfo( std::string& dest_buffer );

//	void StartStreamThread();

	void ThreadMain() { StreamMain(); }

	/// Use this as a thread main loop?
	void StreamMain();

	inline void SetRequestedState( SoundSource::State state );

	void OnCreated();

	void OnReleased();
};


// -------------------------- inline implementations --------------------------

inline SoundSource::State COpenALStreamedSoundSourceImpl::GetRequestedState()
{
	std::lock_guard<std::mutex> scoped_lock(m_StreamSoundMutex);

	const SoundSource::State state = m_RequestedState;

	return state;
}


inline void COpenALStreamedSoundSourceImpl::SetRequestedState( SoundSource::State state )
{
	std::lock_guard<std::mutex> scoped_lock(m_StreamSoundMutex);

	m_RequestedState = state;
}


/// Needs a SoundBuffer object to play a sound
/// - handle pause request from user and system differently
class COpenALNonStreamedSoundSourceImpl : public COpenALSoundSourceImpl
{
	ALuint m_uiBuffer;

public:

	void Release();

	/// TODO: implement fade effect
	void Play( double fadein_time );

	/// TODO: implement fade effect
	void Stop( double fadeout_time );

	/// TODO: implement fade effect
	void Pause( double fadeout_time );

	/// TODO: implement fade effect
	void Resume( double fadein_time );

	void SetLoop( bool loop );

	SoundSource::StreamType GetStreamType() { return SoundSource::NonStreamed; }

	void GetTextInfo( std::string& dest_buffer );
};

} // namespace amorphous



#endif /* __OpenALSoundSourceImpl_H__ */
