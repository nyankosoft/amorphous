#ifndef  __OpenALSoundSourceImpl_H__
#define  __OpenALSoundSourceImpl_H__


#include "fwd.h"
#include "SoundSource.h"
#include "Sound/SoundManagerImpl.h"
#include "Support/SerializableStream.hpp"
#include "Support/prealloc_pool.h"
#include "Support/Macro.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <al.h>
#include <alut.h>
#include <vorbis/vorbisfile.h>


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


class COpenALSoundSourceImpl : public CSoundSourceImpl
{
	/// set by COpenALSoundManagerImpl
	CSoundSource::Management m_ManagementType;

	CSoundSource::Type m_SourceType;

	pooled_object_handle m_BufferHandle;

	bool m_Released;

	/// used by prealloc_pool
	int m_StockIndex;
	int m_StockID;

protected:

	COpenALSoundManagerImpl *m_pManager;

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

	CSoundSource::Type GetSoundType() { return m_SourceType; }

	CSoundSource::Management GetManagementType() { return m_ManagementType; }

	virtual void OnCreated() { m_Released = false; }

	virtual void OnReleased();

	CSoundSource::State GetState();

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

	friend class COpenALSoundManagerImpl;
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

	boost::shared_ptr<boost::thread> m_pThread;

	/// stores an encoded data loaded from disk
	/// - Not used when the sound is streamed directly from the disk
	stream_buffer m_Buffer;

	int m_NumTotalBuffersProcessed;

	CSoundSource::State m_RequestedState;

	bool m_ExitStreamThread;

private:

	bool OpenOrLoadOggResource( const std::string& resource_path,
								OggVorbis_File& sOggVorbisFile,
								CSerializableStream& src_buffer );

	inline CSoundSource::State GetRequestedState();

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

	CSoundSource::StreamType GetStreamType() { return CSoundSource::Streamed; }

	void GetTextInfo( std::string& dest_buffer );

//	void StartStreamThread();

	void ThreadMain() { StreamMain(); }

	/// Use this as a thread main loop?
	void StreamMain();

	inline void SetRequestedState( CSoundSource::State state );

	void OnCreated();

	void OnReleased();
};


// -------------------------- inline implementations --------------------------

inline CSoundSource::State COpenALStreamedSoundSourceImpl::GetRequestedState()
{
	boost::mutex::scoped_lock scoped_lock(m_StreamSoundMutex);

	const CSoundSource::State state = m_RequestedState;

	return state;
}


inline void COpenALStreamedSoundSourceImpl::SetRequestedState( CSoundSource::State state )
{
	boost::mutex::scoped_lock scoped_lock(m_StreamSoundMutex);

	m_RequestedState = state;
}


/// Needs a CSoundBuffer object to play a sound
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

	CSoundSource::StreamType GetStreamType() { return CSoundSource::NonStreamed; }

	void GetTextInfo( std::string& dest_buffer );
};


#endif /* __OpenALSoundSourceImpl_H__ */
