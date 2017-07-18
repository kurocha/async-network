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
#include <Async/Protocol/Stream.hpp>

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
					
					auto endpoints = Endpoint::service_endpoints(51200, SOCK_STREAM);
					
					Fiber server([&]{
						Fiber::Pool bindings;
						
						for (auto & endpoint : endpoints) {
							bindings.resume([&]{
								Fiber::Pool connections;
								
								std::cerr << "binding to " << endpoint.address() << std::endl;
								
								auto socket = endpoint.bind();
								
								socket.listen();
								
								std::cerr << "Socket " << (Descriptor)socket << " listening to " << endpoint.address() << std::endl;
								
								while (true) {
									connections.resume([&, client = socket.accept(reactor)]{
										std::cerr << "Socket " << (Descriptor)socket << " client connected to " << endpoint.address() << std::endl;
										
										Protocol::Stream protocol(client, reactor);
										
										auto message = protocol.read(12);
										
										examiner.expect(message) == "Hello World!";
									});
								}
							});
						}
						
						Fiber::current->yield();
					});
					
					server.resume();
					
					Fiber client([&]{
						for (auto & endpoint : endpoints) {
							auto socket = endpoint.connect(reactor);
							
							std::cerr << "Socket " << (Descriptor)socket << " connected to " << endpoint.address() << std::endl;
							
							Protocol::Stream protocol(socket, reactor);
							
							protocol.write("Hello World!");
						}
					});
					
					client.resume();
					
					reactor.wait(0.1);
				}
			},
		};
	}
}
