// tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Mmsystem.h"
#pragma comment (lib,"Winmm.lib")
#include <fstream>      // std::ifstream
#include "Header.h"
using namespace std;
int main()
{
	cout << "START!\n";
	std::string teststring = "INVITE sip:088124388498@172.25.20.10 SIP/2.0\nFrom: <sip:74956265201@172.25.19.25>;tag=43708020858\nTo: <sip:088124388498@172.25.20.10>\nCall-ID: 35385606432\nCSeq: 11 INVITE\nVia: SIP/2.0/UDP 172.25.19.25:5060\nContact: <sip:74956265201@172.25.19.25>\nMax-Forwards: 70\nAllow: INVITE, CANCEL, ACK, BYE, OPTIONS, INFO, REFER\nContent-Type: application/sdp\nSupported: replaces, timer\nContent-Length: 213\n\nv=0\no=Flat_Contact 0 0 IN IP4 172.25.19.25\ns=SIPCall\nc=IN IP4 172.25.19.25\nt=0 0\nm=audio 29178 RTP/AVP 8 101\na=rtpmap:8 PCMA/8000\na=rtpmap:101 telephone-event/8000\na=fmtp:101 0-15\na=ptime:10\na=sendrecv";
	SIP sip;
	sip.sip = "INVITE sip:088124388498@172.25.20.10 SIP/2.0\nFrom: <sip:74956265201@172.25.19.25>;tag=43708020858\nTo: <sip:088124388498@172.25.20.10>\nCall-ID: 35385606432\nCSeq: 11 INVITE\nVia: SIP/2.0/UDP 172.25.19.25:5060\nContact: <sip:74956265201@172.25.19.25>\nMax-Forwards: 70\nAllow: INVITE, CANCEL, ACK, BYE, OPTIONS, INFO, REFER\nContent-Type: application/sdp\nSupported: replaces, timer\nContent-Length: 213\n\nv=0\no=Flat_Contact 0 0 IN IP4 172.25.19.25\ns=SIPCall\nc=IN IP4 172.25.19.25\nt=0 0\nm=audio 29178 RTP/AVP 8 101\na=rtpmap:8 PCMA/8000\na=rtpmap:101 telephone-event/8000\na=fmtp:101 0-15\na=ptime:10\na=sendrecv";
	sip.Parse();
	//sip.print();
	sip.SDP = "SDP";
	//cout << "\nanswer:\n" << sip.ResponseRING();
	cout << "\nanswer:\n" << sip.ResponseOK();
	//cout << "\n+++" << sip.SDP << "+++\n";
	cout << "\nEND!";
	system("pause");
	
	return 0;
}