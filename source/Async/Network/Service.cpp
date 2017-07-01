//
//  Service.cpp
//  File file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Service.hpp"

#include <string>

namespace Async
{
	namespace Network
	{
		Service::Service(Port port) : _name(std::to_string(port))
		{
		}
		
		Service::~Service()
		{
		}
	}
}
