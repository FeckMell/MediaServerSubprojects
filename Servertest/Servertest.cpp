// Servertest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Data.h"

using boost::asio::ip::udp;
using namespace std;

string MakeMess(Caller C);

boost::asio::io_service service;
std::shared_ptr<udp::socket> Sock;
udp::endpoint EndPoint;
const int port = 2427;
const int porttester = 19000;
const int Users = 5;
int Counter = 0;
//-*/----------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian"/*"ru_RU.UTF-8"*/);
	Sock.reset(new boost::asio::ip::udp::socket(service));
	Sock->open(udp::v4());
	Sock->set_option(boost::asio::ip::udp::socket::reuse_address(true));
	Sock->bind(udp::endpoint(udp::v4(), porttester));
	EndPoint = udp::endpoint(boost::asio::ip::address::from_string("10.77.7.19"), port);

	RequestMGCP mess1;
	RequestSDP mess2;
	vector<Caller> VecCall;
	for (int i = 0; i < Users; ++i)
	{
		if (i <= 1) VecCall.push_back(Caller(0, 29001 + i * 2));
		else VecCall.push_back(Caller(3, 29001 + i * 2));
	}
	VecCall[0].Starter = true;

	cout << "\nInit DONE";
	system("pause");
	return 0;
}
//-*/----------------------------------------------------------
string MakeMess(Caller C)
{
	RequestMGCP MGCPbase;
	RequestSDP SDPbase;
	string MGCP="";
	string SDP="";
	switch (C.mode) 
	{
	case 0:
	{
		MGCP = str(boost::format(string(MGCPbase.AnnCRCX)) % Counter % (C.Event + C.EventNum) % C.CallID%C.MGCPmode);
		SDP = str(boost::format(string(SDPbase.SDP))%C.CallID%C.IP%C.port%C.SDPmode);
		Counter++;
		C.mode = 1;
	}//0
	case 1:
	{
		MGCP = str(boost::format(string(MGCPbase.AnnRQNT)) % Counter % (C.Event + C.EventNum));
		Counter++;
		if(C.Starter) C.mode = 3;
		else C.mode = 4;
	}//1


	}
}
//-*/----------------------------------------------------------
//-*/----------------------------------------------------------
//-*/----------------------------------------------------------
//-*/----------------------------------------------------------
//-*/----------------------------------------------------------
//-*/----------------------------------------------------------
//-*/----------------------------------------------------------