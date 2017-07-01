//
//  Endpoint.cpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Endpoint.hpp"

#include <system_error>

namespace Async
{
	namespace Network
	{
		Endpoint::Endpoint(const Address & address, Socket::Domain socket_domain, Socket::Type socket_type, Socket::Protocol socket_protocol) : _address(address), _socket_domain(socket_domain), _socket_type(socket_type), _socket_protocol(socket_protocol)
		{
		}
		
		Endpoint::~Endpoint()
		{
		}
		
		Endpoints Endpoint::service_endpoints(const Service & service, Socket::Type socket_type)
		{
			struct addrinfo hints = {0};

			// set-up hints structure
			hints.ai_family = AF_UNSPEC;
			hints.ai_flags = AI_PASSIVE; /* listening address */
			hints.ai_socktype = socket_type;

			return for_name(nullptr, service.name().c_str(), &hints);
		}
		
		Endpoints Endpoint::named_endpoints(const std::string & host, const Service & service, Socket::Type socket_type)
		{
			struct addrinfo hints = {0};

			hints.ai_socktype = socket_type;
			hints.ai_family = AF_UNSPEC;

			return for_name(host.c_str(), service.name().c_str(), &hints);
		}
		
		Endpoint::Endpoint(const addrinfo * address_info) : _address(address_info->ai_addr, address_info->ai_addrlen), _socket_domain(address_info->ai_family), _socket_type(address_info->ai_socktype), _socket_protocol(address_info->ai_protocol)
		{
			
		}
		
		Endpoints Endpoint::for_name(const char * host, const char * service, addrinfo * hints)
		{
			struct addrinfo * current, * first;

			auto error = getaddrinfo(host, service, hints, &first);

			if (error) {
				throw std::system_error(error, std::generic_category(), gai_strerror(error));
			}
			
			Endpoints endpoints;
			current = first;
			
			while (current) {
				endpoints.push_back(current);

				current = current->ai_next;
			}

			freeaddrinfo(first);

			return endpoints;
		}
	}
}
