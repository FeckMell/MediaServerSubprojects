// LogParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <sstream>
#include <fstream>

using namespace std;

int main()
{
	std::ifstream filename("LOGS_Mixer.txt");
	std::ofstream output("Logs_parsed.txt");
	std::string temp;
	string a1 = "before wait time passed";
	string a2 = "no data from port";
	string a3 = "it took";
	string a4 = "In receive: Exception:114in thread";
	string a5 = "received something>12";
	string a6 = "";
	while (std::getline(filename, temp)) 
	{
		temp.insert(0, "\n");
		std::size_t found;
		/*found = temp.find(a1);
		if (found != std::string::npos)
			output.write(temp.c_str(), temp.size());
		found = temp.find(a2);
		if (found != std::string::npos)
			output.write(temp.c_str(), temp.size());
		found = temp.find(a4);
		if (found != std::string::npos)
			output.write(temp.c_str(), temp.size());*/
		found = temp.find(a5);
		if (found != std::string::npos)
			output.write(temp.c_str(), temp.size());

		output.flush();
		temp = "";
	}





	system("pause");
}