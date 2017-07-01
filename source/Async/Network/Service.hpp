//
//  Service.hpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include <string>

namespace Async
{
	namespace Network
	{
		/// A TCP/UDP port number
		typedef int Port;
		
		class Service
		{
		public:
			Service(Port port);
			Service(const std::string & name) : _name(name) {}
			Service(const char * name) : Service(std::string(name)) {}
			
			~Service();
			
			const std::string & name() const {return _name;}
			
		private:
			std::string _name;
		};
	}
}
