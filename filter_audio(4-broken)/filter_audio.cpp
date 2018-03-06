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
	temp = "v=0\no=-0 0 IN IP4 127.0.0.1\ns=Kalimb\nc=IN IP4 10.77.7.19\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29008 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2";
	audioPathStr.push_back(temp);
	temp = "v=0\no=-0 0 IN IP4 127.0.0.1\ns=222\nc=IN IP4 192.168.1.123\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 29004 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2";
	audioPathStr.push_back(temp);
	temp = "v=0\no=-0 0 IN IP4 127.0.0.1\ns=111\nc=IN IP4 192.168.1.123\nt=0 0\na=tool:libavformat 57.3.100\nm=audio 20000 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2";
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