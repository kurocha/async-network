//
//  Socket.hpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include <Async/Handle.hpp>

#include "Address.hpp"

namespace Async
{
	class Reactor;
	
	namespace Network
	{
		class Socket : public Handle
		{
		public:
			/// PF_* eg PF_INET
			typedef int Domain;
			
			/// SOCK_STREAM, SOCK_DGRAM, SOCK_RAW
			typedef int Type;
			
			/// IPPROTO_TCP, IPPROTO_UDP, IPPROTO_IPX, etc
			typedef int Protocol;
			
			Socket() {}
			
			Socket(Domain domain, Type type, Protocol protocol = 0);
			Socket(Descriptor descriptor);

			Socket(const Socket &) = default;
			Socket & operator=(const Socket &) = default;

			Socket(Socket &&) = default;
			Socket & operator=(Socket &&) = default;
			
			Domain domain() const;
			Type type() const;
			Protocol protocol() const;
			
			/// Check if the underlying socket is connected to a remote peer using getpeername.
			bool is_connected() const;

			/// Shutdown the read end of the socket.
			void shutdown_read() {shutdown(SHUT_RD);}
			/// Shutdown the write end of the socket.
			void shutdown_write() {shutdown(SHUT_WR);}
			
			/// Shutdown either end of the socket, or both.
			void shutdown(int mode = SHUT_RDWR);
			
			Address local_address() const;
			Address remote_address() const;
			
			void set_reuse_address(bool value = true);
			
			void bind(const Address & address);
			void listen(std::size_t backlog = SOMAXCONN);
			
			void connect(const Address & address, Reactor & reactor);
			
			Socket accept(Reactor & reactor) const;
			
		protected:
			void check_errors();
		};
	}
}
