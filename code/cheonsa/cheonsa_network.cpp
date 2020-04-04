#include "cheonsa_network.h"
#include "cheonsa__ops.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> // for networking on windows.
#include <ws2tcpip.h> // for additional networking junk.
#else
#include <sys/socket.h> // for networking on linux.
#include <netinet/in.h> // for networking on linux.
#include <fcntl.h> // for networking on linux.
#endif

#include <cassert>

// legacy API design is so ugly.
// how does the entire internet run on this cruft?
// unix?

namespace cheonsa
{

	//// maps to stupid name of sockaddr.
	//struct socket_address
	//{
	//	unsigned short    sa_family;    // address family, AF_xxx
	//	char              sa_data[14];  // 14 bytes of protocol address
	//};

	//// 4 bytes, big endian.
	//struct ip_address_v4
	//{
	//	uint8_c bytes[ 4 ];
	//};

	//// maps to stupid name of sockaddr_in dangus.
	//struct socket_address_internet_v4
	//{
	//	short int          sin_family;  // Address family, AF_INET
	//	unsigned short int sin_port;    // Port number
	//	struct in_addr     sin_addr;    // Internet address
	//	unsigned char      sin_zero[ 8 ]; // padding to make it .
	//};

	//// 16 bytes, big endian.
	//struct ip_address_v6
	//{
	//	uint8_c bytes[ 16 ];   // IPv6 address
	//};

	//// maps to stupid dangus name of sockaddr_in6.
	//struct socket_address_internet_v6
	//{
	//	uint16_c sin6_family;   // address family, AF_INET6
	//	uint16_c sin6_port;     // port number, Network Byte Order
	//	uint32_c sin6_flowinfo; // IPv6 flow information
	//	struct ip_address_v6 sin6_addr; // IPv6 address
	//	uint32_c sin6_scope_id; // Scope ID
	//};

#if defined( cheonsa_platform_windows )
	WSADATA network_wsa_data;
#endif

	boolean_c network_start()
	{
#if defined( cheonsa_platform_windows )
		return WSAStartup( MAKEWORD( 2, 2 ), &network_wsa_data ) == NO_ERROR;
#else
		return false
#endif
	}

	void_c network_stop()
	{
#if defined( cheonsa_platform_windows )
		WSACleanup();
#endif
	}

	network_ip4_c::network_ip4_c()
		: bytes{ 0, 0, 0, 0 }
	{
	}

	boolean_c network_ip4_c::from_string( string8_c const & string )
	{
		if ( InetPtonA( AF_INET, string.character_list.get_internal_array(), bytes ) == 1 )
		{
			return true;
		}
		return false;
	}

	boolean_c network_ip4_c::from_string( string16_c const & string )
	{
		if ( InetPtonW( AF_INET, string.character_list.get_internal_array(), bytes ) == 1 )
		{
			return true;
		}
		return false;
	}

	void_c network_ip4_c::to_string( string8_c & string )
	{
		char8_c buffer[ 16 ];
		assert( InetNtopA( AF_INET, bytes, buffer, 16 ) != NULL );
		string = buffer;
	}

	void_c network_ip4_c::to_string( string16_c & string )
	{
		char16_c buffer[ 16 ];
		assert( InetNtopW( AF_INET, bytes, buffer, 16 ) != NULL );
		string = buffer;
	}

	network_ip6_c::network_ip6_c()
		: bytes{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	{
	}

	boolean_c network_ip6_c::from_string( string8_c const & string )
	{
		if ( InetPtonA( AF_INET6, string.character_list.get_internal_array(), bytes ) == 1 )
		{
			return true;
		}
		return false;
	}

	boolean_c network_ip6_c::from_string( string16_c const & string )
	{
		if ( InetPtonW( AF_INET6, string.character_list.get_internal_array(), bytes ) == 1 )
		{
			return true;
		}
		return false;
	}

	void_c network_ip6_c::to_string( string8_c & string )
	{
		char8_c buffer[ 46 ];
		assert( InetNtopA( AF_INET6, bytes, buffer, 46 ) != NULL );
		string = buffer;
	}

	void_c network_ip6_c::to_string( string16_c & string )
	{
		char16_c buffer[ 46 ];
		assert( InetNtopW( AF_INET6, bytes, buffer, 46 ) != NULL );
		string = buffer;
	}

	network_port_c::network_port_c()
		: bytes{ 0, 0 }
	{
	}

	boolean_c network_port_c::from_string( string8_c const & string )
	{
		uint16_c value;
		if ( ops::convert_string8_to_uint16( string, value ) )
		{
			bytes[ 0 ] = static_cast< uint8_c >( value );
			bytes[ 1 ] = static_cast< uint8_c >( value >> 8 );
			return true;
		}
		return false;
	}

	boolean_c network_port_c::from_string( string16_c const & string )
	{
		uint16_c value;
		if ( ops::convert_string8_to_uint16( string8_c( string ), value ) )
		{
			bytes[ 0 ] = static_cast< uint8_c >( value );
			bytes[ 1 ] = static_cast< uint8_c >( value >> 8 );
			return true;
		}
		return false;
	}

	void_c network_port_c::to_string( string8_c & string )
	{
		uint16_c value = static_cast< uint16_c >( bytes[ 0 ] ) | static_cast< uint16_c >( bytes[ 1 ] ) << 8;
		ops::convert_uint16_to_string8( value, string );
	}

	void_c network_port_c::to_string( string16_c & string )
	{
		string8_c result;
		uint16_c value = static_cast< uint16_c >( bytes[ 0 ] ) | static_cast< uint16_c >( bytes[ 1 ] ) << 8;
		ops::convert_uint16_to_string8( value, result );
		string = result;
	}

	network_address_c::network_address_c()
		: type( type_e_none )
		, port()
	{
	}

	network_address_c::network_address_c( network_ip4_c ip4, network_port_c port )
		: type( type_e_ip4 )
		, ip4( ip4 )
		, port( port )
	{
	}

	network_address_c::network_address_c( network_ip6_c ip6, network_port_c port )
		: type( type_e_ip6 )
		, ip6( ip6 )
		, port( port )
	{
	}

	boolean_c network_address_c::operator == ( network_address_c const & other ) const
	{
		if ( type == other.type )
		{
			if ( type == type_e_ip4 )
			{
				for ( sint32_c i = 0; i < 4; i++ )
				{
					if ( ip4.bytes[ i ] != other.ip4.bytes[ i ] )
					{
						return false;
					}
				}
				return true;
			}
			else if ( type == type_e_ip6 )
			{
				for ( sint32_c i = 0; i < 16; i++ )
				{
					if ( ip6.bytes[ i ] != other.ip6.bytes[ i ] )
					{
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}

	boolean_c network_address_c::operator != ( network_address_c const & other ) const
	{
		return ! operator == ( other );
	}

	network_socket_c::network_socket_c()
		: _socket_handle( 0 )
	{
	}

	boolean_c network_socket_c::open( uint16_c port )
	{
		_socket_handle = socket( AF_INET6, SOCK_DGRAM, IPPROTO_UDP );
		if ( _socket_handle <= 0 )
		{
			_socket_handle = 0;
			return false;
		}


		sockaddr_in address_ip4;
		address_ip4.sin_family = AF_INET;
		address_ip4.sin_port = htons( port );
		address_ip4.sin_addr.s_addr = INADDR_ANY;
		memset( address_ip4.sin_zero, 0, sizeof( address_ip4.sin_zero ) );

		if ( bind( _socket_handle, reinterpret_cast< const sockaddr * >( &address_ip4 ), sizeof( sockaddr_in ) ) < 0 )
		{
			close();
			return false;
		}

#if defined( cheonsa_platform_windows )
		DWORD non_blocking = 1;
		if ( ioctlsocket( _socket_handle, FIONBIO, &non_blocking ) != 0 )
		{
			close();
			return false;
		}
#else
		int non_blocking = 1;
		if ( fcntl( _handle, F_SETFL, O_NONBLOCK, non_blocking ) == -1 )
		{
			close();
			return false;
		}
#endif

		return true;
	}

	boolean_c network_socket_c::send( network_address_c & address, void_c * data, uint16_c data_size )
	{
		assert( data );
		assert( data_size > 0 );

		if ( address.type == network_address_c::type_e_ip4 )
		{
			sockaddr_in address_ip4;
			address_ip4.sin_family = AF_INET;
			ops::memory_copy( address.ip4.bytes, &address_ip4.sin_addr, 4 );
			ops::memory_copy( address.port.bytes, &address_ip4.sin_port, 2 );
			memset( address_ip4.sin_zero, 0, sizeof( address_ip4.sin_zero ) );

			int bytes_sent = sendto( _socket_handle, reinterpret_cast< char const * >( data ), data_size, 0, reinterpret_cast< sockaddr const * >( &address_ip4 ), sizeof( sockaddr_in ) );

			return bytes_sent == data_size;
		}
		else if ( address.type == network_address_c::type_e_ip6 )
		{
			sockaddr_in6 address_ip6;
			address_ip6.sin6_family = AF_INET6;
			ops::memory_copy( address.ip6.bytes, &address_ip6.sin6_addr, 16 );
			ops::memory_copy( address.port.bytes, &address_ip6.sin6_port, 2 );
		}

		return false;
	}

	boolean_c network_socket_c::receive( network_address_c & address, void_c * data, uint16_c data_size, uint16_c & data_size_received )
	{
		sockaddr_storage from; // works for ipv4 and ipv6.
		int from_size = sizeof( from );

		int result = recvfrom( _socket_handle, reinterpret_cast< char * >( data ), data_size, 0, reinterpret_cast< sockaddr * >( & from ), & from_size );
		if ( result == SOCKET_ERROR )
		{
			// error was encountered.
			int error_code = WSAGetLastError();
		}
		else if ( result == 0 )
		{
			// connection was gracefully closed.
		}

		if ( data_size_received <= 0 )
		{
			return false;
		}

		if ( from.ss_family == AF_INET )
		{
			address.type = network_address_c::type_e_ip4;
			ops::memory_copy( &reinterpret_cast< sockaddr_in * >( &from )->sin_addr, address.ip4.bytes, 4 );
			ops::memory_copy( &reinterpret_cast< sockaddr_in * >( &from )->sin_port, address.port.bytes, 2 );
		}
		else if ( from.ss_family == AF_INET6 )
		{
			address.type = network_address_c::type_e_ip6;
			ops::memory_copy( &reinterpret_cast< sockaddr_in6 * >( &from )->sin6_addr, address.ip6.bytes, 16 );
			ops::memory_copy( &reinterpret_cast< sockaddr_in6 * >( &from )->sin6_port, address.port.bytes, 2 );
		}

		return true;
	}

	boolean_c network_socket_c::close()
	{
#if defined( cheonsa_platform_windows )
		closesocket( _socket_handle );
#else
		close( _handle );
#endif
		_socket_handle = 0;
		return true;
	}

	/*
	network_connection_c::network_connection_c()
		: _socket()
		, _address()
		, _time_out( 30.0f )
		, _time_since_last_receive( 0.0f )
		, _time_since_last_send( 0.0f )
	{
	}

	boolean_c network_connection_c::connect( network_socket_c * socket, network_address_c & address, float32_c time_out )
	{
		if ( ( socket == nullptr ) || ( _address.type == network_address_c::type_e_none ) )
		{
			return false;
		}
		_socket = socket;
		_address = address;
		_time_out = time_out;
		_time_since_last_receive = 0.0f;
		_time_since_last_send = 0.0f;
		return true;
	}

	boolean_c network_connection_c::disconnect()
	{
		_socket = 0;
		_address.type = network_address_c::type_e_none;
		return true;
	}

	boolean_c network_connection_c::update( float32_c time_step )
	{
		if ( _address.type == network_address_c::type_e_none )
		{
			return false;
		}
		_time_since_last_receive += time_step;
		_time_since_last_send += time_step;
		if ( _time_since_last_receive > _time_out )
		{
			_address.type = network_address_c::type_e_none;
			_address.ip4 = 0;
			return false;
		}
		else if ( _time_since_last_send > _time_out * 0.25f )
		{
			send( "hey!", 4 );
		}
		return false;
	}

	boolean_c network_connection_c::send( void_c * data, uint16_c data_size )
	{
		if ( _address.type == network_address_c::type_e_none )
		{
			return false;
		}
		else if ( !_socket->send( _address, data, data_size ) )
		{
			return false;
		}
		_time_since_last_send = 0.0f;
		return true;
	}

	boolean_c network_connection_c::receive( void_c * data, uint16_c data_size, uint16_c & received_size )
	{
		network_address_c from;
		if ( !_socket->receive( from, data, data_size, received_size ) )
		{
			return false;
		}
		else if ( from != _address )
		{
			return false;
		}
		_time_since_last_receive = 0.0f;
		return true;
	}
	*/

}
