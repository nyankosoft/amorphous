#ifndef  __BASEENTITYHANDLE_H__
#define  __BASEENTITYHANDLE_H__


namespace amorphous
{

class BaseEntity;

class BaseEntityHandle
{
	/// avoid dynamic allocation when the name is short
	enum { CACHE_SIZE = 24 };

	/// holds whether ( m_pBaseEntity == NULL ) means the handle is not initialized,
	/// or the base entity name is invalid and not found in base entity list
	int m_State;

	/// stores a short base entity name
	char m_acBaseEntityName[CACHE_SIZE];

	char *m_pcBaseEntityName;
	
	/// pointer to the base entity
	BaseEntity *m_pBaseEntity;

public:

	BaseEntityHandle() : m_State(STATE_INVALID), m_pcBaseEntityName(NULL), m_pBaseEntity(NULL)
	{
		memset( m_acBaseEntityName, '\0', sizeof(m_acBaseEntityName) );
	}

	BaseEntityHandle( const char* pcBaseEntityName ) : m_State(STATE_UNINITIALIZED), m_pcBaseEntityName(NULL), m_pBaseEntity(NULL)
	{
		memset( m_acBaseEntityName, '\0', sizeof(m_acBaseEntityName) );

		SetBaseEntityName( pcBaseEntityName );
	}

	~BaseEntityHandle() { if( m_pcBaseEntityName ) delete [] m_pcBaseEntityName; }

	inline void SetBaseEntityPointer( BaseEntity *pBaseEntity ) { m_pBaseEntity = pBaseEntity; }

	inline BaseEntity *GetBaseEntityPointer() { return m_pBaseEntity; }

	inline const char *GetBaseEntityName() const
	{
		if( m_pcBaseEntityName )
			return m_pcBaseEntityName;
		else
			return m_acBaseEntityName;
	}

	inline void SetBaseEntityName( const char *pcBaseEntityName )
	{
		if( m_pcBaseEntityName )
		{
			delete [] m_pcBaseEntityName;
			m_pcBaseEntityName = NULL;
		}

		if( strlen(pcBaseEntityName) <= 15 )
		{
			// short enough to store in the fixed sized buffer
			strcpy( m_acBaseEntityName, pcBaseEntityName );
		}
		else
		{
			m_pcBaseEntityName = new char [strlen(pcBaseEntityName) + 1];
			strcpy( m_pcBaseEntityName, pcBaseEntityName );
			m_acBaseEntityName[0] = '\0';
		}

		m_State = STATE_UNINITIALIZED;

		// clear the pointer when a new name is set
		m_pBaseEntity = NULL;
	}

	void SetState( int state ) { m_State = state; }

	int GetState() const { return m_State; }

	BaseEntityHandle& operator=( const BaseEntityHandle& handle )
	{
		SetBaseEntityName( handle.GetBaseEntityName() );

		return *this;
	}

	BaseEntityHandle( const BaseEntityHandle& handle )
		: m_State(STATE_INVALID), m_pcBaseEntityName(NULL), m_pBaseEntity(NULL)
	{
		SetBaseEntityName( handle.GetBaseEntityName() );
	}

	enum eState
	{
		STATE_UNINITIALIZED,
		STATE_VALID,
		STATE_INVALID,
		NUM_STATES
	};
};

} // namespace amorphous


#endif		/*  __BASEENTITYHANDLE_H__  */