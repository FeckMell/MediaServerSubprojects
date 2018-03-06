#include "stdafx.h"
using namespace std;
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
struct MGCP
{
	MGCP(string req)
	{
		mgcp = req;
		error = 0;
		Parse();
	}
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	string ResponseOK(int code, string end)
	{
		auto response = std::to_string(code) + " " + std::to_string(stoi(MessNum) + 1) + " OK";
		if (end != "")
		{
			response += "\nZ: " + end + EventEx.substr(EventEx.find("@"));
			response += "\nI: " + std::to_string(rand() % 100000);
		}
		return response;
	}
	//-------------------------------------------------------------------------------------
	string ResponseBAD(int code, string message)
	{
		auto response = std::to_string(code) + " " + std::to_string(stoi(MessNum) + 1) + " BAD";
		if (message != "")
		{
			response += "\nZ: " + message;
		}
		return response;
	}
	//-------------------------------------------------------------------------------------
	void Parse()
	{
		parseCMD();
		EventP();
		ParamM();
		ParamL();
		ParamI();
		ParamZ();
		ParamC();
		auto fd = mgcp.find("v=0");
		if (fd != std::string::npos){SDP = mgcp.substr(fd);}
	}
	//-------------------------------------------------------------------------------------
	void parseCMD()
	{
		string temp = mgcp.substr(0, 4);
		if (temp == "RQNT"){CMD = "RQNT";}
		else if (temp == "MDCX"){CMD = "MDCX";}
		else if (temp == "CRCX"){CMD = "CRCX";}
		else if (temp == "DLCX"){CMD = "DLCX";}
		else{error = -1;}
	} //DONE
	//-------------------------------------------------------------------------------------
	void EventP()
	{
		auto fd = mgcp.find("ann/");
		if (fd != std::string::npos)
		{
			Event = "ann/";
		}
		else if (mgcp.find("cnf/") != std::string::npos)
		{
			Event = "cnf/";
			fd = mgcp.find("cnf/");
		}
		else { error = -1; return; }
		if (mgcp.find("@") == std::string::npos) { error = -1; return; }
		EventNum = mgcp.substr(fd + 4, mgcp.find("@") - fd - 4);
		EventEx = mgcp.substr(fd, mgcp.find("] ", fd) - fd+1);
		if (mgcp.substr(5, 2) == "  "){MessNum = mgcp.substr(7, fd - 8);}
		else{ MessNum = mgcp.substr(6, fd - 7); }
	}
	//-------------------------------------------------------------------------------------
	void ParamM()
	{
		if (mgcp.find("confrnce") != std::string::npos){paramM = "confrnce";}
		else if (mgcp.find("inactive") != std::string::npos){ paramM = "inactive"; }
	}
	//-------------------------------------------------------------------------------------
	void ParamL()
	{
		auto fd = mgcp.find("L:");
		if (fd != std::string::npos)
		{
			paramL = mgcp.substr(fd + 3, mgcp.find("\n", fd) - fd-3);
		}
		else { paramL = ""; }
	}
	//-------------------------------------------------------------------------------------
	void ParamI()
	{
		auto fd = mgcp.find("I:");
		if (fd != std::string::npos)
		{
			paramI = mgcp.substr(fd + 3, mgcp.find("\n", fd) - fd-3);
		}
		else { paramI = ""; }
	}
	//-------------------------------------------------------------------------------------
	void ParamZ()
	{
		auto fd = mgcp.find("Z:");
		if (fd != std::string::npos)
		{
			paramZ = mgcp.substr(fd + 3, mgcp.find("\n", fd) - fd-3);
		}
		else { paramZ = ""; }
	}
	//-------------------------------------------------------------------------------------
	void ParamC()
	{
		auto fd = mgcp.find("C:");
		if (fd != std::string::npos)
		{
			paramC = mgcp.substr(fd + 3, mgcp.find("\n", fd) - fd-3);
		}
		else { paramC = ""; }
	}
	//-------------------------------------------------------------------------------------
	bool Valid(){ if (error == -1) return false; else return true; }
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//TODO check validity
	string mgcp;

	string CMD;//
	string Event; //cnf/,ann/
	string EventNum;//cnf/n
	string EventEx;//cnf/1@[10.77.7.19]
	string paramM;
	string paramL;
	string paramI;
	string paramZ;
	string paramC;
	string MessNum;
	string SDP;
	int error;
};
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
