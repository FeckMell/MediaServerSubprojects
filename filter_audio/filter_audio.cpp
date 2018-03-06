// FFMPEGtest2.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "Structs.h"
#include "RTPReceive.h"
#include "Ann.h"
//#include "CFilter.h"
//-------------------------------------------------------------------------
int main(int argc, const char * argv[])
{
	boost::asio::io_service io_service_;
	//auto a = CRTPReceive(true);
	vector<string> audioPathStr;
	string temp;
	temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=Kalimba\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29000 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	audioPathStr.push_back(temp);
	temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=I'll Be Gone\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29002 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	audioPathStr.push_back(temp);
	temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=10-rain_dogs\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29004 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	audioPathStr.push_back(temp);
	string SDP = "v=0\no=- 6666 9 IN IP4 10.77.8.5\ns=SIP Call\nc=IN IP4 10.77.8.5\nt=0 0\nm=audio 12100 RTP/AVP 8\a=sendrecv\na=ptime:20\na=rtpmap:8 PCMA/8000";
	int my_port = 16000;
	string filepath = "C:\\Projects\\MGCP\\Release\\MediaFiles\\music_alaw.wav";
	SHP_Ann a(new Ann(SDP, my_port));
	cout << "\ncreated";
	a->Send(filepath);
	//SHP_CRTPReceive a(new CRTPReceive(audioPathStr));
	//a->process_all();

	system("pause");
	printf("FINISHED\n");

	return 0;
}

//-------------------------------------------------------------------------