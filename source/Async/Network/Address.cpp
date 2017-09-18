//
//  Address.cpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Address.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sstream>
#include <cassert>
#include <cstring>
#include <system_error>

#include <iostream>

namespace Async
{
	namespace Network
	{
		Address::Address(const struct sockaddr * data, std::size_t size)
		{
			set(data, size);
		}
		
		Address::~Address()
		{
		}
		
		Address::Address(const Address & other)
		{
			set(other.data(), other.size());
		}
		
		Address & Address::operator=(const Address & other)
		{
			set(other.data(), other.size());
			
			return *this;
		}
		
		int Address::compare(const Address & other) const noexcept
		{
			if (_size < other._size) {
				return -1;
			} else if (_size > other._size) {
				return 1;
			} else {
				return std::memcmp(data(), other.data(), _size);
			}
		}
		
		void Address::set(const struct sockaddr * data, std::size_t size)
		{
			assert(data != nullptr);
			assert(size <= sizeof(_data));

			std::memcpy(&_data, data, size);
			_size = size;
		}
		
		int Address::name_info_for_address(std::string * name, std::string * service, int flags) const
		{
			char * name_buffer = nullptr, * service_buffer = nullptr;
			int name_buffer_size = 0, service_buffer_size = 0;

			if (name != nullptr) {
				name_buffer = reinterpret_cast<char *>(alloca(NI_MAXHOST));
				name_buffer_size = NI_MAXHOST;
			}

			if (service != nullptr) {
				service_buffer = reinterpret_cast<char *>(alloca(NI_MAXSERV));
				service_buffer_size = NI_MAXSERV;
			}

			auto error = getnameinfo(data(), size(), name_buffer, name_buffer_size, service_buffer, service_buffer_size, flags);

			if (error != 0) return error;

			if (name_buffer)
				*name = std::string(name_buffer);

			if (service_buffer)
				*service = std::string(service_buffer);

			/* no error */
			return 0;
		}
		
		Port Address::port() const
		{
			std::string port_string;

			auto error = name_info_for_address(nullptr, &port_string, NI_NUMERICSERV);

			if (error) {
				throw std::system_error(error, std::generic_category(), gai_strerror(error));
			}
			
			return std::stoi(port_string);
		}

		std::string Address::service_name(bool numeric) const
		{
			std::string port_string;
			int error = 0;
			
			if (!numeric) {
				error = name_info_for_address(nullptr, &port_string, NI_NAMEREQD);
			}
			
			if (error == EAI_NONAME || numeric) {
				error = name_info_for_address(nullptr, &port_string, NI_NUMERICSERV);
			}

			if (error) {
				throw std::system_error(error, std::generic_category(), gai_strerror(error));
			}

			return port_string;
		}

		std::string Address::canonical_name(bool numeric) const
		{
			std::string host_string;
			
			int error = 0;
			
			if (!numeric) {
				error = name_info_for_address(&host_string, nullptr, NI_NAMEREQD);
			}
			
			if (error == EAI_NONAME || numeric) {
				error = name_info_for_address(&host_string, nullptr, NI_NUMERICHOST);
			}

			if (error) {
				throw std::system_error(error, std::generic_category(), gai_strerror(error));
			}

			return host_string;
		}
		
		std::ostream & operator<<(std::ostream & output, const Address & address)
		{
			if (address.family() == AF_INET6) {
				output << '[' << address.canonical_name() << "]:";
			} else {
				output << address.canonical_name() << ":";
			}
			
			output << address.service_name();
			
			return output;
		}
	}
}
