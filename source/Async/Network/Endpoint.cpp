//
//  Endpoint.cpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Endpoint.hpp"

#include <system_error>
#include <stdexcept>

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

			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = socket_type;

			return for_name(host.c_str(), service.name().c_str(), &hints);
		}
		
		Endpoints Endpoint::named_endpoints(const URI::Generic & uri)
		{
			// Figure out the service using the scheme, or override with port if specified:
			Service service{uri.scheme};
			if (!uri.port.empty())
				service = uri.port;
			
			// Default to SOCK_STREAM (tcp), override by supplying fragment.
			Socket::Type socket_type = SOCK_STREAM;
			if (!uri.fragment.empty())
				socket_type = socket_type_for_name(uri.fragment);
			
			return named_endpoints(uri.hostname(), service, socket_type);
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
		
		Socket::Type Endpoint::socket_type_for_name(const std::string & name)
		{
			if (name == "tcp" || name == "stream") {
				return SOCK_STREAM;
			} else if (name == "udp" || name == "udp") {
				return SOCK_DGRAM;
			} else if (name == "raw") {
				return SOCK_RAW;
#ifdef SOCK_RDM
			} else if (name == "rdm") {
				return SOCK_RDM;
#endif
			} else {
				throw std::invalid_argument("Unknown socket type name!");
			}
		}
	}
}
