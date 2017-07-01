//
//  Test.Socket.cpp
//  This file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 2/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Concurrent/Fiber.hpp>
#include <Async/Network/Endpoint.hpp>
#include <Async/Network/Socket.hpp>
#include <Async/Reactor.hpp>

#include <list>

namespace Async
{
	namespace Network
	{
		using Concurrent::Fiber;
		
		UnitTest::Suite SocketTestSuite {
			"Async::Network::Socket",
			
			{"it should have some real tests",
				[](UnitTest::Examiner & examiner) {
					Reactor reactor;
					auto endpoints = Endpoint::service_endpoints(2048, SOCK_STREAM);
					
					Fiber server([&](){
						std::list<Fiber> fibers;
						
						for (auto & endpoint : endpoints) {
							fibers.emplace_back([&](){
								auto socket = endpoint.bind();
								
								std::cerr << "Socket " << (Descriptor)socket << " bound to " << endpoint.address() << std::endl;
							});
						}
						
						for (auto & fiber : fibers) {
							fiber.resume();
						}
						
						Fiber::current->yield();
					});
					
					server.resume();
					
					reactor.wait(2);
				}
			},
		};
	}
}
