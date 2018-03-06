// SocketTest.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "CSockRec.h"
//--------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Started\n";
	boost::asio::io_service io_service;

	CSockRec s1(io_service, 30000/*,29187,"10.77.7.19"*/);
	cout << "\n30000 started";
	CSockRec s2(io_service, 30004/*,29187,"10.77.7.19"*/);
	cout << "\n30004 started";
	CSockRec s3(io_service, 30008/*,29187,"10.77.7.19"*/);
	cout << "\n30008 started";
	CSockRec s4(io_service, 30012/*,29187,"10.77.7.19"*/);
	cout << "\n30012 started";

	io_service.run();

	system("pause");
	return 0;
}

