// FFMPEGtest2.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "RTPReceive.h"
//#include "CFilter.h"
//-------------------------------------------------------------------------
int main(int argc, const char * argv[])
{

	//auto a = CRTPReceive(true);
	vector<string> audioPathStr;
	string temp;
	temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=Kalimba\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 27000 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	audioPathStr.push_back(temp);
	temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=I'll Be Gone\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 27002 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	audioPathStr.push_back(temp);
	temp = "\n\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=10-rain_dogs\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 27004 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\n";
	audioPathStr.push_back(temp);
	SHP_CRTPReceive a(new CRTPReceive(audioPathStr));
	//system("pause");
	//filt1->process_all();
	a->process_all();
	system("pause");
	printf("FINISHED\n");

	return 0;
}

//-------------------------------------------------------------------------