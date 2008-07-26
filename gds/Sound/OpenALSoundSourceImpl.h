#ifndef  __OpenALSoundSourceImpl_H__
#define  __OpenALSoundSourceImpl_H__


#include "fwd.h"
#include "SoundSource.h"
#include "Sound/SoundManagerImpl.h"
#include "Support/stream_buffer.h"
#include "Support/prealloc_pool.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <al.h>
#include <alut.h>



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
	// set by COpenALSoundManagerImpl
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

public:

	COpenALSoundSourceImpl();

	virtual ~COpenALSoundSourceImpl();

	void SetPosition( const Vector3& vPosition );

	void SetVelocity( const Vector3& vVelocity );

	void SetPose( const Matrix34& pose );

	void SetLoop( bool loop );

	bool IsDone();

	CSoundSource::Type GetSoundType() { return m_SourceType; }

	CSoundSource::Management GetManagementType() { return m_ManagementType; }

	void OnReleased();

	CSoundSource::State GetState();

	void CreateSource();

	void ReleaseSource();

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

	StreamMethod m_StreamMethod;


	ALuint m_uiBuffers[NUM_MAX_BUFFERS_FOR_STREAMING];


	int m_NumBuffersForStreaming;

	/// How long the thread sleeps 
	int	m_ServiceUpdatePeriodMS;

	boost::mutex m_SoundOperationMutex;

	boost::thread *m_pThread;

	/// stores an encoded data loaded from disk
	/// - Not used when the sound is streamed directly from the disk
	stream_buffer m_Buffer;

public:

	COpenALStreamedSoundSourceImpl();

	void Release();

	void Play( double fadein_time );

	void Stop( double fadeout_time );

	void Pause( double fadeout_time );

	void Resume( double fadein_time );

	CSoundSource::StreamType GetStreamType() { return CSoundSource::Streamed; }

	/// Use this as a thread main loop?
	int StreamMain();
};



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

	CSoundSource::StreamType GetStreamType() { return CSoundSource::NonStreamed; }
};


#endif /* __OpenALSoundSourceImpl_H__ */
