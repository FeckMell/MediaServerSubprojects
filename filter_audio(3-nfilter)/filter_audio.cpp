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
	SHP_CRTPReceive a(new CRTPReceive(true));
	//system("pause");
	//filt1->process_all();
	a->process_all();
	system("pause");
	printf("FINISHED\n");

	return 0;
}
//-------------------------------------------------------------------------