#include "stdafx.h"
#include "CSockRec.h"
//----------------------------------------------------------------------------------------------
CSockRec::CSockRec(boost::asio::io_service& io_service, short my_port)
: io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), my_port))
{
	//ff1.open("result.dat");
	my_port_ = my_port;
	do_receive();
	count = 0;
}
//----------------------------------------------------------------------------------------------
void CSockRec::do_receive()
{
	std::cout << "2";
	//boost::asio::ip::udp::endpoint destination(boost::asio::ip::address::from_string("10.77.7.19"), 29000);
	socket_.async_receive_from(
		boost::asio::buffer(data_, max_length), sender_endpoint_, &(this->do_receive)
		/*[this](boost::system::error_code ec, std::size_t bytes_recvd)
	{
		size_t RTP_HEADER_SIZE = 12;
		if (bytes_recvd > RTP_HEADER_SIZE)
		{
			++count;
			if (count == 100) { count = 0; cout << "\nrecieved 100 from " << this->my_port_; }
			//cout << data_;
			//ff1.write((const char*)data_, strlen((const char*)data_));
			//flush(ff1);
			//const size_t szPack = bytes_recvd - RTP_HEADER_SIZE;
			//SHP_CAVPacket shpPacket = std::make_shared<CAVPacket>(szPack);
			//memcpy(shpPacket->data, data_ + RTP_HEADER_SIZE, szPack);
			
			//circular_.push(shpPacket);
			//cout << "\ncircular_.push(shpPacket): " << shpPacket->data;
			std::cout << "1";
		}
		//do_receive();
		
	}*/);
	
	
}