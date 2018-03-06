#include "stdafx.h"
#include "Header.h"
SIP::SIP(string){ SIPLines.resize((int)line::lineMax); }
//*///------------------------------------------------------------------------------------------
//*///------------------------------------------------------------------------------------------
//*///------------------------------------------------------------------------------------------
std::string SIP::ResponseBAD(int code, string message){ return " "; }
//*///------------------------------------------------------------------------------------------
//*///------------------------------------------------------------------------------------------
//*///------------------------------------------------------------------------------------------
void SIP::Parse()
{
	sip = string(mes);
	Remove();
	ParseToLines();
}
//*///------------------------------------------------------------------------------------------
void SIP::Remove()
{
	auto fd = sip.find("\r");
	while (fd != std::string::npos)
	{
		sip.erase(sip.begin() + fd);
		fd = sip.find("\r", fd - 1);
	}
	fd = sip.find("  ");
	while (fd != std::string::npos)
	{
		sip.erase(sip.begin() + fd);
		fd = sip.find("  ", fd - 1);
	}
}
//*///------------------------------------------------------------------------------------------
void SIP::ParseCMD()
{
	if (SIPLines[FirstLine].find("INVITE") != std::string::npos) CMD = INVITE;
	else if (SIPLines[FirstLine].find("ACK") != std::string::npos) CMD = ACK;
	else if (SIPLines[FirstLine].find("BYE") != std::string::npos) CMD = BYE;
	else if (SIPLines[FirstLine].find("OK") != std::string::npos) CMD = OK;
}
//*///------------------------------------------------------------------------------------------
void SIP::ParseToLines()
{
	SIPLines[FirstLine] = sip.substr(0, sip.find("\n"));
	SIPLines[Via] = get_substr(sip, "Via: ", "\n");
	SIPLines[To] = get_substr(sip, "To: ", "\n");
	SIPLines[From] = get_substr(sip, "From: ", "\n");
	SIPLines[CallID] = get_substr(sip, "Call-ID: ", "\n");
	SIPLines[CSeq] = get_substr(sip, "CSeq: ", "\n");
	SIPLines[Contact] = get_substr(sip, "Contact: ", "\n");
	SIPLines[ContentLen] = get_substr(sip, "Content-Length: ", "\n");
	SIPLines[CintentType] = get_substr(sip, "Content-Type: ", "\n");
	SIPLines[MaxForwards] = get_substr(sip, "Max-Forwards: ", "\n");
	SIPLines[Supported] = get_substr(sip, "Supported: ", "\n"); 
	SIPLines[Allow] = get_substr(sip, "Allow: ", "\n");
	SDP = sip.substr(sip.find("v=0"));
}
//*///------------------------------------------------------------------------------------------
void SIP::print()
{
	AddTagTo();
	cout << "\nPrinting:";
	for (int i = 0; i < (int)line::lineMax; ++i)
		cout << "\n+++" << SIPLines[i]<<"+++\n";
	cout << "print finished!";
}
//*///------------------------------------------------------------------------------------------
void SIP::AddTagTo()
{
	auto a = std::chrono::steady_clock::now();
	SIPLines[To] += ";tag=" + std::to_string(a.time_since_epoch().count() % 10000000000);
}
//*///------------------------------------------------------------------------------------------
std::string SIP::ResponseOK()
{
	std::string result = "SIP/2.0 200 OK\n";
	result += "Via: " + SIPLines[Via] + ViaBranch + ViaReceived + "\n";
	result += "To: " + SIPLines[To] + "\n";
	result += "From: " + SIPLines[From] + "\n";
	result += "Call-ID: " + SIPLines[CallID] + "\n";
	result += "CSeq: " + SIPLines[CSeq] + "\n";
	result += "Contact: " + MyContact + "\n";
	result += "Content-Type: application/sdp\n";
	result += "Content-Length: " + std::to_string(SDP.length())+"\n";
	result += "\n" + SDP;
	return result;
}
//*///------------------------------------------------------------------------------------------
std::string SIP::ResponseRING()
{
	AddTagTo();
	std::string result = "SIP/2.0 180 Ringing\n";
	result += "Via: " + SIPLines[Via] + ViaReceived + "\n";
	result += "To: " + SIPLines[To] + "\n";
	result += "From: " + SIPLines[From] + "\n";
	result += "Call-ID: " + SIPLines[CallID] + "\n";
	result += "CSeq: " + SIPLines[CSeq] + "\n";
	result += "Contact: " + MyContact + "\n";
	result += "Content-Length: 0";

	return result;
}
//*///------------------------------------------------------------------------------------------
std::string SIP::ResponseTRY(int code, std::string mess){}
//*///------------------------------------------------------------------------------------------
//*///------------------------------------------------------------------------------------------
//*///------------------------------------------------------------------------------------------


















































































std::string get_substr(std::string target, std::string aim, std::string fin)
{
	auto fd = target.find(aim);
	std::string result = "";
	if (fd != std::string::npos)
		result = target.substr(fd + aim.size(), target.find(fin, fd + aim.size() - 1) - (fd + aim.size()));
	return result;
}