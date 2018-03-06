#pragma once
#include "stdafx.h"
#include "FFStructs.h"
#include "TemplateStack.h"
#include <fstream>


class CSockRec
{
public:
	CSockRec(boost::asio::io_service& io_service, short port);
private:
	void do_receive();

	short my_port_;
	udp::endpoint sender_endpoint_;
	boost::asio::io_service& io_service_;
	//CThreadedCircular<SHP_CAVPacket> circular_{ 100, false }; // {..} - параметры. 50 -размер буффера. Поток завершен? - нет -false
	udp::socket socket_;
	enum { max_length = 2048 };
	unsigned char data_[max_length];
	ofstream ff1;
	int count;
};
typedef shared_ptr<CSockRec> SHP_CSockRec;