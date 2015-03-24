#ifndef __amorphous_AES_HPP__
#define __amorphous_AES_HPP__


#include <string>
#include <vector>
#include "OpenAES/oaes_lib.h"
#include "Log/DefaultLog.hpp"


namespace amorphous
{


class AES
{
	OAES_CTX *m_ctx;

	int GenerateXbitKey( unsigned int key_bit_length )
	{
		OAES_RET ret = OAES_RET_ERROR;

		switch( key_bit_length )
		{
		case 128: ret = oaes_key_gen_128(m_ctx); break;
		case 192: ret = oaes_key_gen_192(m_ctx); break;
		case 256: ret = oaes_key_gen_256(m_ctx); break;
		default:
			return -1;
			break;
		}

		LOG_PRINTF_ERROR(( "oaes_key_gen_%u() failed: %d, ", key_bit_length, (int)ret ));

		return (ret == OAES_RET_SUCCESS) ? 0 : -1;
	}

public:

	AES() : m_ctx(NULL)
	{
		m_ctx = oaes_alloc();

		if( m_ctx == NULL )
			LOG_PRINT_ERROR( "oaes_alloc() returned NULL." );
	}

	/**
	 @brief Constructor that initializes the instance and imports an encryption key.

	 @param[in] key An encryption key. Note that a 256-bit key does not mean key.size() == 32
	 */
	AES( std::vector<unsigned char>& key ) : m_ctx(NULL)
	{
		m_ctx = oaes_alloc();

		if( m_ctx == NULL )
			LOG_PRINT_ERROR( "oaes_alloc() returned NULL." );

		if( key.empty() )
		{
			LOG_PRINT_WARNING( "key.empty()" );
			return;
		}

		OAES_RET ret = oaes_key_import( m_ctx, &key[0], key.size() );

		if( ret != OAES_RET_SUCCESS )
			LOG_PRINTF_ERROR(( "oaes_key_import() failed: %d", (int)ret ));
	}

	~AES()
	{
		if( !m_ctx )
			return;

		OAES_RET ret = oaes_free( &m_ctx );

		if( ret != OAES_RET_SUCCESS )
		{
			LOG_PRINTF_ERROR(( "oaes_free() failed: %d", (int)ret ));
		}
	}

//	int GenerateKey( unsigned int key_bit_length )
//	{
//	}

	int GenerateKey( unsigned int key_bit_length, std::vector<unsigned char>& key )
	{
		if( m_ctx == NULL )
			LOG_PRINT_ERROR( "m_ctx == NULL." );

//		OAES_RET ret = oaes_key_gen_256(m_ctx);
		int r = GenerateXbitKey(key_bit_length);

//		if( ret != OAES_RET_SUCCESS )
		if( r != 0 )
		{
//			LOG_PRINTF_ERROR(( "oaes_key_gen_256() failed: %d", (int)ret ));
			return -1;
		}

		size_t key_data_size = 0;

		OAES_RET ret = oaes_key_export(m_ctx, NULL, &key_data_size);

		if( ret != OAES_RET_SUCCESS )
		{
			LOG_PRINTF_ERROR(( "Failed to retrieve key length: %d", (int)ret ));
			return -1;
		}

		key.resize( 0 );
		key.resize( key_data_size, 0 );

		ret = oaes_key_export(m_ctx, &key[0], &key_data_size);

		if( ret != OAES_RET_SUCCESS )
		{
			LOG_PRINTF_ERROR(( "Failed to export key: %d", (int)ret ));
			return -1;
		}

		return 0;
	}

//	void SetKey( std::vector<unsigned char>& key );

	int Encrypt( const std::vector<unsigned char>& plaintext, std::vector<unsigned char>& ciphertext )
	{
		if( plaintext.size() == 0 )
			return -1;

		if( m_ctx == NULL )
		{
			LOG_PRINT_ERROR( "m_ctx == NULL." );
			return -1;
		}

		size_t encryption_buffer_size = 0;

		OAES_RET ret = oaes_encrypt( m_ctx, (const uint8_t *)&plaintext[0], plaintext.size(), NULL, &encryption_buffer_size );

		if( ret != OAES_RET_SUCCESS )
		{
			LOG_PRINTF_ERROR(( "Failed to retrieve required buffer size for encryption: %d", (int)ret ));
			return -1;
		}

		if( encryption_buffer_size == 0 )
		{
			LOG_PRINT_ERROR( "The required encryption buffer size == 0." );
			return -1;
		}

		ciphertext.resize( 0 );
		ciphertext.resize( encryption_buffer_size );

		ret = oaes_encrypt( m_ctx, (const uint8_t *)&plaintext[0], plaintext.size(), &ciphertext[0], &encryption_buffer_size );

		if( ret != OAES_RET_SUCCESS )
		{
			LOG_PRINTF_ERROR(( "Encryption failed: %d", (int)ret ));
			return -1;
		}

		return 0;
	}

	int Decrypt( const std::vector<unsigned char>& src, std::vector<unsigned char>& dest )
	{
		return 0;
	}
};


} // namespace my_namespace


#endif /* __amorphous_AES_HPP__ */
