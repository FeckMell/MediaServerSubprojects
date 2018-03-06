#include "stdafx.h"
std::string get_substr(std::string target, std::string aim, std::string fin);

class SIP
{
public:
	//func
	enum Eventor { INVITE, ACK, BYE, Ringing, OK };
	enum line{ FirstLine, Via, To, From, CallID, CSeq, Contact, ContentLen, CintentType, MaxForwards, Allow, Supported, lineMax };

	SIP(std::string);
	SIP() { SIPLines.resize((int)line::lineMax); }

	std::string ResponseINVITE(int code, std::string end);
	std::string ResponseOK();
	std::string ResponseRING();
	std::string ResponseTRY(int code, std::string mess);
	std::string ResponseBAD(int code, std::string message);

	void print();
	void Parse();
	void Remove();
	void AddTagTo();
	void ParseToLines();
	void ParseCMD();

	std::string sip;
	Eventor CMD;//
	std::string SDP="";
	std::string MyContact = "<sip:1001@172.27.0.6:5060;maddr=172.27.0.6>";
	std::string ViaReceived = ";received=10.77.7.19";
	std::string ViaBranch = ";branch=111111111;";
	std::vector<std::string> SIPLines;
	boost::asio::ip::udp::endpoint sender;
	int error;
	char mes[2049];
private:
	
};