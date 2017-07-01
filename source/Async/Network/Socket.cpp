//
//  Socket.cpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Socket.hpp"

#include <system_error>

#include <Async/Readable.hpp>

namespace Async
{
	namespace Network
	{
		Socket::Socket(Domain domain, Type type, Protocol protocol) : Handle(::socket(domain, type, protocol))
		{
			set_non_blocking(*this);
		}
		
		Socket::~Socket()
		{
		}
		
		void Socket::shutdown(int mode)
		{
			auto result = ::shutdown(_descriptor, mode);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "shutdown");
		}
		
		Address Socket::local_address()
		{
			sockaddr_storage storage;
			sockaddr * data = reinterpret_cast<sockaddr *>(&storage);
			socklen_t size = sizeof(storage);

			auto result = ::getsockname(_descriptor, data, &size);

			if (result == -1 || errno == ENOTCONN)
				throw std::system_error(errno, std::generic_category(), "getsockname");
			
			return Address(data, size);
		}
		
		Address Socket::remote_address()
		{
			sockaddr_storage storage;
			sockaddr * data = reinterpret_cast<sockaddr *>(&storage);
			socklen_t size = sizeof(storage);

			auto result = ::getpeername(_descriptor, data, &size);

			if (result == -1 || errno == ENOTCONN)
				throw std::system_error(errno, std::generic_category(), "getpeername");
			
			return Address(data, size);
		}
		
		void Socket::bind(Address & address)
		{
			auto result = ::bind(_descriptor, address.data(), address.size());
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "bind");
		}
		
		void Socket::listen(std::size_t backlog)
		{
			auto result = ::listen(_descriptor, backlog);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "listen");
		}
		
		Socket Socket::accept(Reactor & reactor)
		{
			Readable event(_descriptor, reactor);
			
			sockaddr_storage storage;
			sockaddr * data = reinterpret_cast<sockaddr *>(&storage);
			socklen_t size = sizeof(storage);
			
			while (true) {
				auto result = ::accept(_descriptor, data, &size);
				
				if (result == -1) {
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						throw std::system_error(errno, std::generic_category(), "accept");
				} else {
					return Socket(result);
				}
				
				event.wait();
			}
		}
	}
}
