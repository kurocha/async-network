//
//  Socket.cpp
//  This file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 2/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Parallel/Distributor.hpp>
#include <Concurrent/Fiber.hpp>
#include <Async/Network/Endpoint.hpp>
#include <Async/Network/Socket.hpp>
#include <Async/Reactor.hpp>
#include <Async/Protocol/Stream.hpp>

#include <Time/Statistics.hpp>
#include <Time/Timer.hpp>

#include <list>
#include <signal.h>

namespace Async
{
	namespace Network
	{
		using Concurrent::Fiber;
		using namespace UnitTest::Expectations;
		
		UnitTest::Suite SocketTestSuite {
			"Async::Network::Socket",
			
			{"it can allocate sockets quickly",
				[](UnitTest::Examiner & examiner) {
					const std::size_t batch_size = 1024;
					
					std::vector<Socket> sockets;
					sockets.reserve(10*batch_size);
					
					Time::Timer timer;
					
					for (std::size_t i = 0; i < 10; i += 1) {
						timer.reset();
						
						for (std::size_t j = 0; j < batch_size; j += 1) {
							sockets.push_back({PF_INET, SOCK_STREAM});
						}
						
						examiner.expect(timer.time()).to(be < Time::Interval(1.0));
						examiner << sockets.size() << " sockets: " << timer.time() << " for " << batch_size << " syscalls." << std::endl;
					}
				}
			},
			
			{"it connects quickly",
				[](UnitTest::Examiner & examiner) {
					signal(SIGPIPE, SIG_IGN);
					
					std::mutex statistics_mutex;
					Time::Statistics client_statistics;
					
					auto endpoints = Endpoint::service_endpoints(0, SOCK_STREAM);
					
					std::vector<Socket> servers;
					
					for (auto && endpoint : endpoints) {
						auto && socket = endpoint.bind();
						
						socket.listen();
						
						servers.push_back(std::move(socket));
					}
					
					Time::Timer duration;
					
					{
						Parallel::Distributor<std::function<void()>> server(1, 1);
						
						for (std::size_t i = 0; i < server.concurrency(); i += 1) {
							server([&servers](){
								Reactor reactor;
								Fiber::Pool bindings;
								
								for (auto && socket : servers) {
									bindings.resume([&]{
										Fiber::current->annotate("server");
										
										while (true) {
											auto && peer = socket.accept(reactor);
											Protocol::Stream protocol(peer, reactor);
											
											auto message = protocol.read(12);
											protocol.write(message);
										}
									});
								}
								
								reactor.wait(1.0);
							});
						}
						
						Parallel::Distributor<std::function<void()>> client(1, 2);
						
						for (std::size_t i = 0; i < client.concurrency(); i += 1) {
							client([&servers, &statistics_mutex, &client_statistics](){
								Time::Statistics statistics;
								
								Reactor reactor;
								Fiber::Pool connections;
								
								for (auto && server : servers) {
									connections.resume([&]{
										Endpoint endpoint(server);
										Fiber::current->annotate("client connection");
										
										while (true) {
											auto sample = statistics.sample();
											
											auto && peer = endpoint.connect(reactor);
											Protocol::Stream protocol(peer, reactor);
											
											protocol.write("Hello World!");
											auto message = protocol.read(12);
										}
									});
									
									break;
								}
								
								reactor.wait(1.0);
								
								{
									std::lock_guard<std::mutex> guard(statistics_mutex);
									client_statistics += statistics;
								}
							});
						}
					}
					
					examiner << "Amortized samples: " << client_statistics.amortized_samples_per_second(duration.time()) << std::endl;
					examiner << "Samples per second: " << client_statistics.samples_per_second() << std::endl;
					examiner << "Minimum duration: " << client_statistics.minimum_duration() << std::endl;
					examiner << "Maximum duration: " << client_statistics.maximum_duration() << std::endl;
					examiner.expect(client_statistics.samples_per_second()).to(be > 100);
				}
			},
			
			{"it can send and receive data",
				[](UnitTest::Examiner & examiner) {
					signal(SIGPIPE, SIG_IGN);
					
					std::mutex statistics_mutex;
					Time::Statistics client_statistics;
					
					auto endpoints = Endpoint::service_endpoints(0, SOCK_STREAM);
					
					std::vector<Socket> servers;
					
					for (auto && endpoint : endpoints) {
						auto && socket = endpoint.bind();
						
						socket.listen();
						
						servers.push_back(std::move(socket));
					}
					
					Time::Timer duration;
					
					{
						Parallel::Distributor<std::function<void()>> server(1, 1);
						
						for (std::size_t i = 0; i < server.concurrency(); i += 1) {
							server([&servers](){
								Reactor reactor;
								Fiber::Pool bindings;
								
								for (auto && socket : servers) {
									bindings.resume([&]{
										Fiber::current->annotate("server");
										
										Fiber::Pool connections;
										
										while (true) {
											auto && peer = socket.accept(reactor);
											
											connections.resume([&, peer]{
												Fiber::current->annotate("server connection");
												Protocol::Stream protocol(peer, reactor);
												
												while (true) {
													auto message = protocol.read(12);
													
													if (message.empty()) break;
													
													protocol.write(message);
												}
											});
										}
									});
								}
								
								reactor.wait(1.0);
							});
						}
						
						Parallel::Distributor<std::function<void()>> client(1, 2);
						
						for (std::size_t i = 0; i < client.concurrency(); i += 1) {
							client([&servers, &statistics_mutex, &client_statistics](){
								Time::Statistics statistics;
								
								Reactor reactor;
								Fiber::Pool connections;
								
								for (auto && server : servers) {
									connections.resume([&]{
										Endpoint endpoint(server);
										
										Fiber::current->annotate("client connection");
										
										while (true) {
											auto sample = statistics.sample();
											
											auto && peer = endpoint.connect(reactor);
											Protocol::Stream protocol(peer, reactor);
											
											protocol.write("Hello World!");
											
											auto message = protocol.read(12);
											
											if (message.empty()) break;
										}
									});
									
									break;
								}
								
								reactor.wait(1.0);
								
								{
									std::lock_guard<std::mutex> guard(statistics_mutex);
									client_statistics += statistics;
								}
							});
						}
					}
					
					examiner << "Amortized samples: " << client_statistics.amortized_samples_per_second(duration.time()) << std::endl;
					examiner << "Samples per second: " << client_statistics.samples_per_second() << std::endl;
					examiner << "Minimum duration: " << client_statistics.minimum_duration() << std::endl;
					examiner << "Maximum duration: " << client_statistics.maximum_duration() << std::endl;
					examiner.expect(client_statistics.samples_per_second()).to(be > 100);
				}
			},
		};
	}
}
