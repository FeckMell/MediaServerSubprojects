#include "stdafx.h"
using namespace std;
extern int Counter;

struct RequestMGCP
{
	//%1% - num %2% - event+eventnum %3% - callid %4%- mode
	string AnnCRCX = "CRCX %1% %2%@[10.77.7.20] mgcp 1.0 ncs 1.0\nC: %3%\nL: a:PCMA\nM: %4%\n";
	string AnnRQNT = "RQNT %1% %2%@[10.77.7.20] mgcp 1.0 ncs 1.0\nX: %3%\nR: oc,of\nS: pa@*(an=vb(sil,null,3),file:///music_alaw.wav)\nQ: loop\n";
	string AnnDLCX = "DLCX %1% %2%@[10.77.7.20] mgcp 1.0 ncs 1.0\nC: %3%\n";

	string ConfCRCX = "CRCX %1% %2%@[10.77.7.20] mgcp 1.0 ncs 1.0\nC: %3%\nL: a:PCMA\nM: %4%\n";
	string ConfMDCX = "MDCX %1% %2%@[10.77.7.20] mgcp 1.0 ncs 1.0\nC: %3%\nI: 1\nL: a:PCMA\nM: %4%\n";
	string ConfDLCX = "DLCX %1% %2%@[10.77.7.20] mgcp 1.0 ncs 1.0\nC: %3%\n";
};
struct RequestSDP
{// %1% - CallID, %2% - IP  %3% - port %4% - mode
	string SDP = "\nv=0\no=OpenStage-Line_0 605284321 968300597 IN IP4 10.77.7.19\ns=%1%\nc=IN IP4 %2%\nt=0 0\nm=audio %3% RTP/AVP 8\na=rtpmap:8 PCMA/8000\na=%4%\n";
	
};

struct Caller
{
	Caller(int m, int p) :mode(m), port(to_string(p))
	{
		if (m <= 2) { Event = "ann/"; MGCPmode = "sendrecv"; }
		else 
		{ 
			Event = "cnf/"; 
			if (m == 4) MGCPmode = "inactive";
			else MGCPmode = "confrnce";
		}
	}
	string port;
	string IP = "10.77.7.19";
	string CallID = port;
	int mode = 0;
	string Event;
	string EventNum="$";
	string MGCPmode;
	string SDPmode;
	bool Starter = false;
};
