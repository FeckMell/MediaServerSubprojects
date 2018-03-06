// CallAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
enum { max_length = 1024 };

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian"/*"ru_RU.UTF-8"*/);

	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: CallAgent <host> <port>\n";
			system("pause");
			return 1;
		}
		auto host = argv[1];
		auto port = argv[2];

		boost::asio::io_service io_service;
		udp::socket s(io_service, udp::endpoint(udp::v4(), 0));
		udp::resolver resolver(io_service);
		printf_s("Resolving params %s %s...\n", host,  port);

		udp::endpoint endpoint = *resolver.resolve({ udp::v4(), host, port});
		printf_s("\thost:\t%s (%s)\n\tport:\t%s\n", 
			host, endpoint.address().to_string().c_str(), port);

		char request[max_length+1];
		size_t request_length=0;

		do
		{
			std::cout << "\nEnter filename or line to send: ";
			std::cin.getline(request, max_length);
			request_length = std::strlen(request);

			FILE* f;
			errno_t err = fopen_s(&f, request, "r");
			if (err == 0)
			{
				auto len = fread_s(request, max_length, 1, max_length, f);
				fclose(f);
				if (!len)
					std::cerr << request << " file read error.\n";
				else
				{
					request[len] = 0;
					std::cout << "File content:\n" << request << "\n";
					request_length = len;
				}
			}
			if(request_length > 1)
			{
				s.send_to(boost::asio::buffer(request, request_length), endpoint);
				udp::endpoint sender_endpoint;
				size_t reply_length = s.receive_from(
					boost::asio::buffer(request, max_length), sender_endpoint);
				request[reply_length] = 0;
				std::cout << "===========Reply is:\n" << request << "\n===========\n";
/*
				std::cout.write(request, reply_length);
				std::cout << "\n";
*/
			}

		} while (request_length > 1);
	}
	catch (std::exception& e)
	{
		//printf("\n test333\n");
		std::cerr << "Exception: " << e.what() << "\n";
		system("pause");
		return 100;
	}
	system("pause");
	return 0;
}

