//
//  Endpoint.hpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Socket.hpp"
#include "Service.hpp"

#include <vector>

#include <netdb.h>

namespace Async
{
	namespace Network
	{
		class Endpoint;
		
		typedef std::vector<Endpoint> Endpoints;
		
		class Endpoint
		{
		public:
			Endpoint(const Address & address, Socket::Domain socket_domain, Socket::Type socket_type, Socket::Protocol socket_protocol);
			virtual ~Endpoint();
			
			Endpoint(const Endpoint & other) = default;
			Endpoint & operator=(const Endpoint & other) = default;
			
			const Address & address() const {return _address;}
			const Socket::Domain & socket_domain() const {return _socket_domain;}
			const Socket::Type & socket_type() const {return _socket_type;}
			const Socket::Protocol & socket_protocol() const {return _socket_protocol;}
			
			static Endpoints service_endpoints(const Service & service, Socket::Type socket_type = SOCK_STREAM);
			static Endpoints named_endpoints(const std::string & host, const Service & service, Socket::Type socket_type = SOCK_STREAM);
			
			Socket bind(bool reuse_address = true)
			{
				Socket socket(_socket_domain, _socket_type, _socket_protocol);
				
				socket.set_reuse_address();
				socket.bind(_address);
				
				return socket;
			}
			
			Socket connect(Reactor & reactor)
			{
				Socket socket(_socket_domain, _socket_type, _socket_protocol);
				
				socket.connect(_address, reactor);
				
				return socket;
			}
			
		private:
			Endpoint(const addrinfo *);
			static Endpoints for_name(const char * host, const char * service, addrinfo * hints);
			
			Address _address;
			
			Socket::Domain _socket_domain = 0;
			Socket::Type _socket_type = 0;
			Socket::Protocol _socket_protocol = 0;
		};
	}
}
