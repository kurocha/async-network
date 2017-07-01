//
//  Test.Address.cpp
//  This file is part of the "Async::Network" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Async/Network/Address.hpp>

#include <arpa/inet.h>

namespace Async
{
	namespace Network
	{
		UnitTest::Suite AddressTestSuite {
			"Async::Network::Address",
			
			{"it can give the correct ipv4 address",
				[](UnitTest::Examiner & examiner) {
					struct sockaddr_in socket_address;
					socket_address.sin_family = AF_INET;
					socket_address.sin_port = htons(12345);
					socket_address.sin_addr.s_addr = inet_addr("192.168.1.2");
					
					Address address(reinterpret_cast<struct sockaddr *>(&socket_address), sizeof(socket_address));
					
					examiner.expect(address.family()) == AF_INET;
					examiner.expect(address.canonical_name()) == "192.168.1.2";
					examiner.expect(address.port()) == 12345;
				}
			}
		};
	}
}
