//
//  Test.Endpoint.cpp
//  This file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 2/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Async/Network/Endpoint.hpp>

namespace Async
{
	namespace Network
	{
		using namespace UnitTest::Expectations;
		
		UnitTest::Suite EndpointTestSuite {
			"Async::Network::Endpoint",
			
			{"it should resolve interface endpoints",
				[](UnitTest::Examiner & examiner) {
					auto endpoints = Endpoint::service_endpoints(1024, SOCK_STREAM);
					
					examiner << "Interface endpoints available." << std::endl;
					examiner.expect(endpoints.size()) > 0u;
					
					int inet = 0;
					for(auto & endpoint : endpoints)
					{
						if (endpoint.address().family() == AF_INET)
							inet += 1;
					}
					
					examiner << "IPv4 address was present." << std::endl;
					examiner.expect(inet) > 0;

					examiner.expect([&](){
						Endpoint::named_endpoints("localhost", "ThisServiceDoesNotExist", SOCK_STREAM);
					}).to(throw_exception<std::system_error>());
				}
			},
			
			{"it should resolve host endpoints",
				[](UnitTest::Examiner & examiner) {
					auto endpoints = Endpoint::named_endpoints("localhost", "http", SOCK_STREAM);
					
					examiner.expect(endpoints.size()) > 0u;
				}
			},
			
			{"it can resolve uri endpoints",
				[](UnitTest::Examiner & examiner) {
					auto endpoints = Endpoint::named_endpoints("http://127.0.0.1");
					
					examiner.expect(endpoints.size()) > 0u;
					
					auto endpoint = endpoints.front();
					examiner.expect(endpoint.socket_domain()) == PF_INET;
					examiner.expect(endpoint.socket_type()) == SOCK_STREAM;
					examiner.expect(endpoint.address().port()) == 80;
				}
			},
		};
	}
}
