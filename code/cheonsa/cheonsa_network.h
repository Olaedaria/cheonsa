#pragma once

#include "cheonsa_types.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"
#include "cheonsa_core_event.h"

// maximum size of data we can send per packet over UDP is 65507 bytes.
// this should be plenty, i don't see a game needing to push it to that limit.
// but checks will be in place to see that that limit is not met.

namespace cheonsa
{

	// needs to be called at engine start up if the engine is going to be using networking.
	boolean_c network_start();

	// needs to be called at engine shut down.
	void_c network_stop();

	class network_ip4_c
	{
	public:
		uint8_c bytes[ 4 ]; // 32-bit unsigned integer, in network byte order (big endian).
		network_ip4_c();
		boolean_c from_string( string8_c const & string );
		boolean_c from_string( string16_c const & string );
		void_c to_string( string8_c & string );
		void_c to_string( string16_c & string );
	};

	class network_ip6_c
	{
	public:
		uint8_c bytes[ 16 ]; // 128-bit unsigned integer, in network byte order (big endian).
		network_ip6_c();
		boolean_c from_string( string8_c const & string );
		boolean_c from_string( string16_c const & string );
		void_c to_string( string8_c & string );
		void_c to_string( string16_c & string );
	};

	class network_port_c
	{
	public:
		uint8_c bytes[ 2 ]; // 16-bit unsigned integer, in network byte order (big endian).
		network_port_c();
		boolean_c from_string( string8_c const & string );
		boolean_c from_string( string16_c const & string );
		void_c to_string( string8_c & string );
		void_c to_string( string16_c & string );
	};

	// stores an internet protocol version 4 address or an internet protocol version 6 address and a port number.
	class network_address_c
	{
	public:
		enum type_e
		{
			type_e_none,
			type_e_ip4,
			type_e_ip6,
		};

		uint16_c type;

		union
		{
			network_ip4_c ip4;
			network_ip6_c ip6;
		};

		network_port_c port;

	public:
		network_address_c();
		network_address_c( network_ip4_c ip4, network_port_c port ); // initializes this address as ip version 4 with port number, values should be in network byte order.
		network_address_c( network_ip6_c ip6, network_port_c port ); // initializes this address as ip version 6 with port number, values should be in network byte order.

		boolean_c operator == ( network_address_c const & other ) const;
		boolean_c operator != ( network_address_c const & other ) const;

	};

	// a socket lets us send anything to any address and recieve anything from any address.
	// this is implemented with user datagram protocol (UDP) socket.
	class network_socket_c
	{
	private:
		uint_native_c _socket_handle;

	public:
		network_socket_c();
		boolean_c open( uint16_c port ); // opens a non-blocking socket for sending and receiving data. returns true on success, false on failure.
		boolean_c send( network_address_c & address, void_c * data, uint16_c data_size ); // this is where we send outgoing data. sends data of data_size in bytes to address. returns true if data is sent (which does not mean that data was received by recipient), false if not.
		boolean_c receive( network_address_c & address, void_c * data, uint16_c data_size, uint16_c & data_size_received ); // this is where we listen for and receive incoming data. when calling, data should be set to a buffer of data_size that is large enough to hold the result. upon returning, sets address to the address of where the incoming data is from, sets data to the value of the incoming data, and sets received_size to the size in bytes of the received data. returns true if incoming data was received and if data_size >= the number of bytes received, false if not.
		boolean_c close();

	};

	//// a connection lets us send anything to one address and recieve anything from one address.
	//// the connection is persistent, in the sense that it will remain open so long as we remain in regular contact with the other address.
	//// keep alive packets will be sent and received to keep the connection alive in the case that the program is not actively sending/receiving anything else through the connection.
	//// the connection will time out if we don't receive anything from the other address after a certain amount of time.
	//class network_connection_c
	//{
	//public:
	//	network_connection_c();

	//	boolean_c connect( network_socket_c * socket, network_address_c & address, float32_c time_out ); // establishes a sending and reciving connection with the given address.
	//	boolean_c disconnect();

	//	boolean_c update( float32_c time_step ); // updates internal timers and sends heart beat packets out to the other address as needed to keep the connection alive on their end. returns false if disconnected, which can happen if we don't hear back from the other address after a while.
	//	boolean_c send( void_c * data, uint16_c data_size ); // returns true if packet send was successful. returns false if otherwise. if it returns true, that doesn't mean that it was receieved on the other end, it just means that it was sent.
	//	boolean_c receive( void_c * data, uint16_c data_size, uint16_c & received_size ); // returns true if a packet was recieved. returns false if otherwise, or if data_size is too small to hold the data of the incoming packet. on call, data_size is the size of data buffer which will recieve the incoming packet data. on return, data_size is set to the size of the incoming packet data.

	//	core_event_c< void_c, network_connection_c * > on_desync; // occurs when connection times out or send_reliable fails.

	//private:
	//	network_socket_c * _socket; // the socket provides the interface for us to send and recieve data over the internet.
	//	network_address_c _address; // the address of who this connection is with. we will only accept packets which originate from this address.
	//	float64_c _time_out; // how long it takes in seconds before we mark our connection with the remote client as disconnected.
	//	float64_c _time_since_last_receive; // tracks how much time has passed since we last receieved anything from the remote client.
	//	float64_c _time_since_last_send; // tracks how much time has passed since we last sent anything to the remote client.

	//};


	//// these connections are used on the server.
	//class network_connection_to_client_c
	//{
	//public:
	//};

	//// these connections are used on the client.
	//class network_connection_to_server_c
	//{
	//};
	//
	//namespace packets
	//{
	//	// sent by client to server to request connection to the server.
	//	class connection_request_c
	//	{
	//	public:
	//		uint64_c client_salt; // random number generated by the client to uniquely identify it behind NAT and stuff.


	//	};

	//	// sent by the server in response to client request to connect.
	//	class connection_response_c
	//	{
	//	public:
	//		uint64_c client_salt; // the server sends back
	//		uint64_c server_salt;

	//	};
	//}

	

}
