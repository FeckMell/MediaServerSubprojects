// Parser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <tchar.h>
#include <winsock2.h> // Wincosk2.h должен быть раньше windows!
#include <windows.h>
#include "Struct.h"
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	string  temp = "DLCX  18 cnf/1@[10.77.7.19] MGCP 1.0\nC: 00000000000000000000000000000002\nL: a:PCMA;PCMU;G729;telephone-event, s:off\nM: confrnce\n";
	temp += "\nv=0\no=- 0 0 IN IP4 127.0.0.1\ns=Kalimba\nc=IN IP4 10.77.7.19\nt=0 0\nm=audio 30000 RTP/AVP 97\nb=AS:705\na=rtpmap:97 PCMU/44100/2\na=sendrecv\n";
	auto t1 = std::chrono::steady_clock::now();
	MGCP obj(temp);
	auto t2 = std::chrono::steady_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	cout << "\n\nPARSED IN:" << dur.count();
	//cout << "\nVALID? ="<<obj.Valid();
	cout << "\nresponse:\n"+obj.ResponseOK(200, "cnf/1")+"\n";
	system("pause");
	return 0;
}

