// FFMPEGtest2.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "RTPReceive.h"
//#include "CFilter.h"
boost::asio::io_service io_service_;
//-------------------------------------------------------------------------
int main(int argc, const char * argv[])
{

	//auto a = CRTPReceive(true);
	vector<string> audioPathStr;
	string temp;
	temp = "\n\nv=0\no=- 0 0 IN IP4 10.77.7.19\ns=Kalimba\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29000 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	//temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=Kalimba\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 5014 RTP/AVP 8 0 18 101\na=rtpmap:8 PCMA/8000\na=rtpmap:0 PCMU/8000\na=rtpmap:18 G729/8000\n";
	//temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=Kalimba\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 5014 RTP/AVP 101 0 8 18\na=rtpmap:0 PCMU/8000\n";
	//temp = "rtp://10.77.7.19:29000?localport=0";
	audioPathStr.push_back(temp);
	temp = "\n\nv=0\no=- 0 0 IN IP4 10.77.7.19\ns=I'll Be Gone\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29002 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	//temp = "rtp://10.77.7.19:29004?localport=0";
	audioPathStr.push_back(temp);
	temp = "\n\nv=0\no=- 0 0 IN IP4 10.77.7.19\ns=10-rain_dogs\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29004 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	//temp = "rtp://10.77.7.19:29008?localport=0";
	audioPathStr.push_back(temp);
	SHP_CRTPReceive a(new CRTPReceive(audioPathStr));
	//system("pause");
	//filt1->process_all();
	a->process_all();
	io_service_.run();
	system("pause");
	printf("FINISHED\n");

	return 0;
}

//-------------------------------------------------------------------------