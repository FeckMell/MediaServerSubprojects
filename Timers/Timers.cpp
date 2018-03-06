// Timers.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <inttypes.h>
#include <chrono>
#include <mutex>
#include <thread>

/*int main()
{
	LARGE_INTEGER Freq, Time, Current;
	QueryPerformanceFrequency(&Freq);
	double Delay = Freq.QuadPart * 0.020;
	for (int i = 0; i < 200; i++) 
	{
		QueryPerformanceCounter(&Time);
		do { QueryPerformanceCounter(&Current); } while (Current.QuadPart - Time.QuadPart < Delay);
		printf("QueryPerformanceCounter() - %fms\n", (Current.QuadPart - Time.QuadPart) * 1000.0 / Freq.QuadPart);
	}
	system("pause");
	return 0;
}*/
int main()
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	LARGE_INTEGER Freq, Time, Current;
	QueryPerformanceFrequency(&Freq);
	double Delay = Freq.QuadPart * 0.020;

	//20 milli = -200000LL
	liDueTime.QuadPart = -200000LL;
	double sum = 0.;

	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	for (int i = 0; i < 1000; ++i)
	{
		auto t1 = std::chrono::steady_clock::now();
		QueryPerformanceCounter(&Time);


		//SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
		//WaitForSingleObject(hTimer, INFINITE);
		//Sleep(20);
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		do { QueryPerformanceCounter(&Current); } while (Current.QuadPart - Time.QuadPart < Delay);


		QueryPerformanceCounter(&Current);
		auto t2 = std::chrono::steady_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "\n" << i << ") ";
		printf("QueryPerformanceCounter() - %fms", (Current.QuadPart - Time.QuadPart) * 1000.0 / Freq.QuadPart);
		sum += (Current.QuadPart - Time.QuadPart) * 1000.0 / Freq.QuadPart;
		std::cout << " std=" << dur.count();
	}
	std::cout << "\n\n\nRESULT:" << sum/1000;
	system("pause");
	return 0;
}

