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
					
					auto endpoints = Endpoint::service_endpoints(4040, SOCK_STREAM);
					
					Fiber server([&](){
						std::list<Fiber> fibers;
						
						for (auto & endpoint : endpoints) {
							fibers.emplace_back([&](){
								std::cerr << "binding to " << endpoint.address() << std::endl;
								
								auto socket = endpoint.bind();
								
								socket.listen();
								
								std::cerr << "Socket " << (Descriptor)socket << " listening to " << endpoint.address() << std::endl;
								
								auto client = socket.accept(reactor);
								
								std::cerr << "Socket " << (Descriptor)socket << " client connected to " << endpoint.address() << std::endl;
								
								Byte buffer[128];
								auto end = client.read(buffer, buffer+127, reactor);
								*(end+1) = '\0';
								
								examiner.expect(std::string((char*)buffer)) == "Hello World!";
							});
						}
						
						for (auto & fiber : fibers) {
							fiber.resume();
						}
						
						Fiber::current->yield();
					});
					
					server.resume();
					
					Fiber client([&](){
						for (auto & endpoint : endpoints) {
							auto socket = endpoint.connect(reactor);
							
							std::cerr << "Socket " << (Descriptor)socket << " connected to " << endpoint.address() << std::endl;
							
							std::string message = "Hello World!";
							socket.write((const Byte *)message.data(), (const Byte *)message.data() + message.size(), reactor);
						}
					});
					
					client.resume();
					
					reactor.wait(2);
				}
			},
		};
	}
}
