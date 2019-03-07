//
//  Socket.cpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Socket.hpp"

#include <sys/socket.h>
#include <system_error>

#include <Async/After.hpp>
#include <Async/Readable.hpp>
#include <Async/Writable.hpp>

#include <fcntl.h>

namespace Async
{
	namespace Network
	{
		Socket::Socket(Descriptor descriptor) : Handle(descriptor)
		{
			update_flags(*this, O_NONBLOCK | O_CLOEXEC);
		}
		
		Socket::Socket(Domain domain, Type type, Protocol protocol) : Socket(::socket(domain, type, protocol))
		{
		}
		
		Socket::Domain Socket::domain() const
		{
			Domain domain = -1;
			socklen_t length = sizeof(domain);
			
			auto result = getsockopt(_descriptor, SOL_SOCKET, SO_DOMAIN, &domain, &length);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "getsockopt(SOL_SOCKET, SO_DOMAIN)");
			
			return domain;
		}
		
		Socket::Type Socket::type() const
		{
			Type type = -1;
			socklen_t length = sizeof(type);
			
			auto result = getsockopt(_descriptor, SOL_SOCKET, SO_TYPE, &type, &length);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "getsockopt(SOL_SOCKET, SO_TYPE)");
			
			return type;
		}
		
		Socket::Protocol Socket::protocol() const
		{
			Protocol protocol = 0;
			socklen_t length = sizeof(protocol);
			
			auto result = getsockopt(_descriptor, SOL_SOCKET, SO_PROTOCOL, &protocol, &length);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "getsockopt(SOL_SOCKET, SO_PROTOCOL)");
			
			return protocol;
		}
		
		void Socket::shutdown(int mode)
		{
			auto result = ::shutdown(_descriptor, mode);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "shutdown");
		}
		
		Address Socket::local_address() const
		{
			sockaddr_storage storage;
			sockaddr * data = reinterpret_cast<sockaddr *>(&storage);
			socklen_t size = sizeof(storage);

			auto result = ::getsockname(_descriptor, data, &size);

			if (result == -1 || errno == ENOTCONN)
				throw std::system_error(errno, std::generic_category(), "getsockname");
			
			return Address(data, size);
		}
		
		Address Socket::remote_address() const
		{
			sockaddr_storage storage;
			sockaddr * data = reinterpret_cast<sockaddr *>(&storage);
			socklen_t size = sizeof(storage);

			auto result = ::getpeername(_descriptor, data, &size);

			if (result == -1 || errno == ENOTCONN)
				throw std::system_error(errno, std::generic_category(), "getpeername");
			
			return Address(data, size);
		}
		
		void Socket::set_reuse_address(bool value)
		{
			int reuse = value ? 1 : 0;
			
			if (::setsockopt(_descriptor, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
				throw std::system_error(errno, std::generic_category(), "setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, ...)");

#ifdef SO_REUSEPORT
			if (::setsockopt(_descriptor, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
					throw std::system_error(errno, std::generic_category(), "setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, ...)");
#endif
		}
		
		void Socket::bind(const Address & address)
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
		
		Socket Socket::accept(Reactor & reactor) const
		{
			Readable event(_descriptor, reactor);
			
			sockaddr_storage storage;
			sockaddr * data = reinterpret_cast<sockaddr *>(&storage);
			socklen_t size = sizeof(storage);
			
			while (true) {
				// std::cerr << "::accept(" << _descriptor << ", ...)" << std::endl;
				auto result = ::accept(_descriptor, data, &size);
				
				if (result == -1) {
					// std::cerr << "::accept(" << _descriptor << ", ...) -> " << result << " errno=" << errno << std::endl;
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						throw std::system_error(errno, std::generic_category(), "accept");
				} else {
					return result;
				}
				
				// std::cerr << "::accept(" << _descriptor << ", ...) waiting..." << std::endl;
				event.wait();
			}
		}
		
		void Socket::connect(const Address & address, Reactor & reactor)
		{
			auto result = ::connect(_descriptor, address.data(), address.size());
			
			// std::cerr << "::connect(...) -> " << result << std::endl;
			
			if (result == -1) {
				if (errno == EINPROGRESS) {
					// connection in progress, wait to be writable.
					Writable event(_descriptor, reactor);
					
					event.wait();
					
					check_errors();
					// std::cerr << "::connect(...) -> connected" << std::endl;
				} else {
					// std::cerr << "::connect(...) -> " << result << errno << std::endl;
					throw std::system_error(errno, std::generic_category(), "connect");
				}
			} else {
				// std::cerr << "::connect(...) -> connected" << std::endl;
				return;
			}
		}
		
		void Socket::check_errors()
		{
			int error = 0;
			socklen_t size = sizeof(error);
			
			auto result = ::getsockopt(_descriptor, SOL_SOCKET, SO_ERROR, &error, &size);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "getsockopt");
			else if (error != 0)
				throw std::system_error(error, std::generic_category(), "SO_ERROR");
		}
	}
}
