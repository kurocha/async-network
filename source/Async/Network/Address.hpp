//
//  Address.hpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 21/10/2007.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Service.hpp"

#include <sys/socket.h>
#include <iosfwd>

namespace Async
{
	namespace Network
	{
		/// AF_INET, AF_INET6
		typedef int AddressFamily;
		
		// A container for sockaddr
		class Address
		{
		public:
			/// Construct from another address and a <tt>sockaddr *</tt>. This is used when receiving a connection, for example, from the bind system call.
			Address(const struct sockaddr * data, std::size_t size);
			~Address();
			
			Address(const Address & other);
			Address & operator=(const Address &);
			
			bool operator==(const Address & other) const noexcept {return compare(other) == 0;};
			bool operator!=(const Address & other) const noexcept {return compare(other) != 0;};
			bool operator<(const Address & other) const noexcept {return compare(other) < 0;};
			bool operator<=(const Address & other) const noexcept {return compare(other) <= 0;};
			bool operator>(const Address & other) const noexcept {return compare(other) > 0;};
			bool operator>=(const Address & other) const noexcept {return compare(other) >= 0;};
			
			void set(const struct sockaddr *, std::size_t size);
			
			/// The internal sockaddr_storage and it's current size.
			sockaddr * data() {return reinterpret_cast<sockaddr *>(&_data);}
			const sockaddr * data() const {return reinterpret_cast<const sockaddr *>(&_data);}
			std::size_t size() const {return _size;}
			
			AddressFamily family() const noexcept {return _data.ss_family;}
			
			/// The port number if it is applicable.
			Port port() const;

			/// The service name if it is applicable. Retrieved from /etc/services based on the port number.
			std::string service_name() const;

			/// Typically returns the hostname if one is available, otherwise returns the numeric address.
			std::string canonical_name(bool numeric = true) const;
			
		private:
			int name_info_for_address(std::string * name, std::string * service, int flags) const;
			
			int compare(const Address & other) const noexcept;
			
			/// Address data(sockaddr)
			sockaddr_storage _data{.ss_family = 0};
			
			/// Address data length
			unsigned _size = 0;
		};
		
		std::ostream & operator<<(std::ostream & output, const Address & address);
	}
}
