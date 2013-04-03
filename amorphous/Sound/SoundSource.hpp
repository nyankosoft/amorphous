#ifndef  __SoundSource_H__
#define  __SoundSource_H__


#include <string>
#include "../3DMath/Matrix34.hpp"


namespace amorphous
{


class SoundSourceImpl;


//====================================================================================
// SoundSource
//====================================================================================

class SoundSource
{
	SoundSourceImpl *m_pImpl;

	int m_StockIndex;
	int m_StockID;

private:
	
	/// set borrowed reference
	/// - Not deleted in dtor
	void SetImpl( SoundSourceImpl *pImpl ) { m_pImpl = pImpl; }

	SoundSourceImpl *GetImpl() { return m_pImpl; }

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

	SoundSource() : m_pImpl(NULL) {}

	~SoundSource() {}

	/// See SoundSourceImpl::OnReleased()
	inline void OnReleased();

	/// See SoundSourceImpl::Release()
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

	inline SoundSource::Type GetSoundType();

	inline SoundSource::StreamType GetStreamType();

	inline SoundSource::Management GetManagementType();

	inline SoundSource::State GetState();

	inline void GetTextInfo( std::string& dest_buffer );

	// functions used by pre_alloc_pool
	// - Not used if the implementation of the sound manager does not use
	//   pre_alloc_pool for memory management of SoundSource

	int GetStockIndex() const { return m_StockIndex; }
	void SetStockIndex( int index ) { m_StockIndex = index; }

	int GetStockID() const { return m_StockID; }
	void SetStockID( int id ) { m_StockID = id; }

	friend class SoundManagerImpl;
};



//====================================================================================
// SoundSourceImpl
//====================================================================================

class SoundSourceImpl
{
public:

	/// Implemented by sound source implementations of OpenAL 
	/// - Called in OpenALSoundManagerImpl::Update() when a source is detached
	///   from the active source list
	virtual void Release() {}

	/// Implemented by sound source implementations of OpenAL 
	/// - Called in OpenALSoundManagerImpl::ReleaseSoundSource()
	virtual void OnReleased() {}

	virtual void Play( double fadein_time ) = 0;

	virtual void Stop( double fadeout_time ) = 0;

	virtual void Pause( double fadeout_time ) = 0;

	virtual void Resume( double fadein_time ) = 0;

	virtual void SetPosition( const Vector3& vPosition ) = 0;

	virtual void SetVelocity( const Vector3& vVelocity ) = 0;

	virtual void SetPose( const Matrix34& pose ) = 0;

	virtual void SetLoop( bool loop ) = 0;

	/// return true if either of the following is true
	/// - sound source needs to be manually released and it has been released.
	/// - sound source is supposed to be automatically released and it has been fiinshed playing.
	virtual bool IsDone() { return false; }

	virtual SoundSource::Type GetSoundType() = 0;

	virtual SoundSource::StreamType GetStreamType() = 0;

	virtual SoundSource::Management GetManagementType() = 0;

	virtual SoundSource::State GetState() = 0;

	virtual void GetTextInfo( std::string& dest_buffer ) {}
};


//=================================== inline implementations ===================================

inline void SoundSource::Release()
{
	m_pImpl->Release();
}

inline void SoundSource::OnReleased()
{
	m_pImpl->OnReleased();
}

inline void SoundSource::Play( double fadein_time )
{
	m_pImpl->Play( fadein_time );
}

inline void SoundSource::Stop( double fadeout_time )
{
	m_pImpl->Stop( fadeout_time );
}

inline void SoundSource::Pause( double fadeout_time )
{
	m_pImpl->Pause( fadeout_time );
}

inline void SoundSource::Resume( double fadein_time )
{
	m_pImpl->Resume( fadein_time );
}

inline void SoundSource::SetPosition( const Vector3& vPosition )
{
	m_pImpl->SetPosition( vPosition );
}

inline void SoundSource::SetVelocity( const Vector3& vVelocity )
{
	m_pImpl->SetVelocity( vVelocity );
}

inline void SoundSource::SetPose( const Matrix34& pose )
{
	m_pImpl->SetPose( pose );
}

inline void SoundSource::SetLoop( bool loop )
{
	m_pImpl->SetLoop( loop );
}

inline bool SoundSource::IsDone()
{
	return m_pImpl->IsDone();
}

inline SoundSource::Type SoundSource::GetSoundType()
{
	return m_pImpl->GetSoundType();
}

inline SoundSource::StreamType SoundSource::GetStreamType()
{
	return m_pImpl->GetStreamType();
}

inline SoundSource::Management SoundSource::GetManagementType()
{
	return m_pImpl->GetManagementType();
}

inline SoundSource::State SoundSource::GetState()
{
	return m_pImpl->GetState();
}

inline void SoundSource::GetTextInfo( std::string& dest_buffer )
{
	return m_pImpl->GetTextInfo( dest_buffer );
}


} // namespace amorphous


#endif /* __SoundSource_H__ */
