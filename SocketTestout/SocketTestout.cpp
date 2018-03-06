// SocketTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


io_service service;
const int size = 160;
const int tracks = 8;

vector<std::shared_ptr<udp::socket>> VecSock;
vector<char*> VecBuf;
vector<char*> VecToSend;
vector<int> VecPort;
vector<udp::endpoint> VecEndpoint;
vector<std::ifstream> VecFile;

void openfiles()
{
	for (int i = 0; i < tracks; ++i)
	{
		string s = to_string(i) + ".wav";
		VecFile[i].open(s.c_str(), std::ifstream::binary);
	}
}
void closefiles()
{
	cout << "\nFiles reopen!";
	for (int i = 0; i < tracks; ++i)
		VecFile[i].close();
}
//--------------------------------------------------------------
void do_send()
{
	openfiles();
	while (true)
	{
		for (int i = 0; i < tracks; ++i)
		{
			if (VecFile[i].read(VecBuf[i], size))
			{
				memcpy(VecToSend[i], VecBuf[i], 12);
				memcpy(VecToSend[i] + 12, VecBuf[i], size);
				VecSock[i]->send_to(boost::asio::buffer(VecToSend[i], size + 12), VecEndpoint[i]);
			}
			else
			{
				closefiles();
				openfiles();
				memcpy(VecToSend[i], VecBuf[i], 12);
				memcpy(VecToSend[i] + 12, VecBuf[i], size);
				VecSock[i]->send_to(boost::asio::buffer(VecToSend[i], size + 12), VecEndpoint[i]);

			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

}
//--------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	timeBeginPeriod(1);
	cout << "Start sending for tracks = " << tracks << "; music packet size = " << size;
	VecSock.resize(tracks);
	VecBuf.resize(tracks);
	VecToSend.resize(tracks);
	VecEndpoint.resize(tracks);
	VecFile.resize(tracks);
	for (int i = 0; i < tracks; ++i)
	{
		VecPort.push_back(29001 + i * 2);
		VecSock[i].reset(new boost::asio::ip::udp::socket(service));
		VecSock[i]->open(udp::v4());
		VecSock[i]->set_option(boost::asio::ip::udp::socket::reuse_address(true));
		VecSock[i]->bind(udp::endpoint(udp::v4(), VecPort[i]));
		VecEndpoint[i] = udp::endpoint(boost::asio::ip::address::from_string("10.77.7.19"), (29000 + i * 2));
		VecBuf[i] = new char[size+1];
		VecToSend[i] = new char[size+12+1];
	}
	do_send();
	
	system("pause");
	return 0;
}

