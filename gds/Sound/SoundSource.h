#ifndef  __SoundSource_H__
#define  __SoundSource_H__


#include "3DMath/Matrix34.h"


class CSoundSourceImpl;


//====================================================================================
// CSoundSource
//====================================================================================

class CSoundSource
{
	CSoundSourceImpl *m_pImpl;

	int m_StockIndex;
	int m_StockID;

private:
	
	/// set borrowed reference
	/// - Not deleted in dtor
	void SetImpl( CSoundSourceImpl *pImpl ) { m_pImpl = pImpl; }

	CSoundSourceImpl *GetImpl() { return m_pImpl; }

public:

	enum Type
	{
		Type_3DSound,
		Type_Non3DSound,
		NumTypes
	};

	enum StreamType
	{
		Streamed,
		NonStreamed,
		NumStreamTypes
	};

	enum Management
	{
		Auto,   ///< released automatically
		Manual, ///< user is responsible for releasing the sound source
		NumManagementTypes
	};

	enum State
	{
		State_Playing,
		State_Stopped,
		State_Paused,
		State_Invalid,
		NumStates
	};

public:

	CSoundSource() : m_pImpl(NULL) {}

	~CSoundSource() {}

	/// See CSoundSourceImpl::OnReleased()
	inline void OnReleased();

	/// See CSoundSourceImpl::Release()
	inline void Release();

	inline void Play( double fadein_time = 0.0f );

	inline void Stop( double fadeout_time = 0.0f );

	inline void Pause( double fadeout_time = 0.0f );

	inline void Resume( double fadein_time = 0.0f );

	inline void SetPosition( const Vector3& vPosition );

	inline void SetVelocity( const Vector3& vVelocity );

	inline void SetPose( const Matrix34& pose );

	inline void SetLoop( bool loop );

	inline bool IsDone();

	inline CSoundSource::Type GetSoundType();

	inline CSoundSource::StreamType GetStreamType();

	inline CSoundSource::Management GetManagementType();

	inline CSoundSource::State GetState();

	// functions used by pre_alloc_pool
	// - Not used if the implementation of the sound manager does not use
	//   pre_alloc_pool for memory management of CSoundSource

	int GetStockIndex() const { return m_StockIndex; }
	void SetStockIndex( int index ) { m_StockIndex = index; }

	int GetStockID() const { return m_StockID; }
	void SetStockID( int id ) { m_StockID = id; }

	friend class CSoundManagerImpl;
};



//====================================================================================
// CSoundSourceImpl
//====================================================================================

class CSoundSourceImpl
{
public:

	/// Implemented by sound source implementations of OpenAL 
	/// - Called in COpenALSoundManagerImpl::Update() when a source is detached
	///   from the active source list
	virtual void Release() {}

	/// Implemented by sound source implementations of OpenAL 
	/// - Called in COpenALSoundManagerImpl::ReleaseSoundSource()
	virtual void OnReleased() {}

	virtual void Play( double fadein_time ) = 0;

	virtual void Stop( double fadeout_time ) = 0;

	virtual void Pause( double fadeout_time ) = 0;

	virtual void Resume( double fadein_time ) = 0;

	virtual void SetPosition( const Vector3& vPosition ) = 0;

	virtual void SetVelocity( const Vector3& vVelocity ) = 0;

	virtual void SetPose( const Matrix34& pose ) = 0;

	virtual void SetLoop( bool loop ) = 0;

	/// return true if either following is true
	/// - sound needs to be manual released and it has been released.
	/// - sound is supposed to be automatically released and it has been fiinshed playing.
	virtual bool IsDone() { return false; }

	virtual CSoundSource::Type GetSoundType() = 0;

	virtual CSoundSource::StreamType GetStreamType() = 0;

	virtual CSoundSource::Management GetManagementType() = 0;

	virtual CSoundSource::State GetState() = 0;
};


//=================================== inline implementations ===================================

inline void CSoundSource::Release()
{
	m_pImpl->Release();
}

inline void CSoundSource::OnReleased()
{
	m_pImpl->OnReleased();
}

inline void CSoundSource::Play( double fadein_time )
{
	m_pImpl->Play( fadein_time );
}

inline void CSoundSource::Stop( double fadeout_time )
{
	m_pImpl->Stop( fadeout_time );
}

inline void CSoundSource::Pause( double fadeout_time )
{
	m_pImpl->Pause( fadeout_time );
}

inline void CSoundSource::Resume( double fadein_time )
{
	m_pImpl->Resume( fadein_time );
}

inline void CSoundSource::SetPosition( const Vector3& vPosition )
{
	m_pImpl->SetPosition( vPosition );
}

inline void CSoundSource::SetVelocity( const Vector3& vVelocity )
{
	m_pImpl->SetVelocity( vVelocity );
}

inline void CSoundSource::SetPose( const Matrix34& pose )
{
	m_pImpl->SetPose( pose );
}

inline void CSoundSource::SetLoop( bool loop )
{
	m_pImpl->SetLoop( loop );
}

inline bool CSoundSource::IsDone()
{
	return m_pImpl->IsDone();
}

inline CSoundSource::Type CSoundSource::GetSoundType()
{
	return m_pImpl->GetSoundType();
}

inline CSoundSource::StreamType CSoundSource::GetStreamType()
{
	return m_pImpl->GetStreamType();
}

inline CSoundSource::Management CSoundSource::GetManagementType()
{
	return m_pImpl->GetManagementType();
}

inline CSoundSource::State CSoundSource::GetState()
{
	return m_pImpl->GetState();
}


#endif /* __SoundSource_H__ */
