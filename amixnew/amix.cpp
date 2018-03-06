// amix.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>

#include "../MGCPserver/DestFusion.h"
#include "../MGCPserver/SrcCommon.h"
#include "../MGCPserver/SrcAsio.h"
#include "../MGCPserver/SrcCash.h"
#include "../MGCPserver/Conf.h"
#include <boost/range.hpp>
#include <boost/thread.hpp>
/*
#define INPUT_SAMPLERATE     44100
#define INPUT_FORMAT         AV_SAMPLE_FMT_S16*/
#pragma region AMIX_origin
#define INPUT_CHANNEL_LAYOUT AV_CH_LAYOUT_STEREO

/** The output bit rate in kbit/s */
//#define OUTPUT_BIT_RATE 44100
/** The number of output channels */
#define OUTPUT_CHANNELS 1
/** The audio sample output format */
#define OUTPUT_SAMPLE_FORMAT AV_SAMPLE_FMT_S8

#define VOLUME_VAL 0.90





struct IN_OUT
{
	string sdp_string;
	unsigned short potIn;
	TRTP_Dest rtpdest;
	string CallID;
};

struct FIN_OUT
{
	string file;
	TRTP_Dest rtpdest;
};

asio::io_service io_service_;

void worker_thread()
{
	io_service_.run();
}
string getSelfIP()
{

	const auto strHostName = asio::ip::host_name();

	boost::system::error_code ec;
	udp::resolver resolver(io_service_);
	udp::resolver::query query(udp::v4(), strHostName, "0");
	udp::resolver::iterator const end, itr = resolver.resolve(query, ec);
	if (ec)
		cout << "Error resolving myself: " << ec.message() << '\n';
	assert(itr != end);

	return itr->endpoint().address().to_string();
}

#define CONFERENCE__

void test_threading();
template <typename T, size_t N> size_t arr_size(T(&)[N]) { return N; }

int main(int argc, const char * argv[])
{
	setlocale(LC_ALL, "Russian"/*"ru_RU.UTF-8"*/);
	//test_threading();
	const string strOutAddr = getSelfIP();// "10.77.7.19";
#ifdef CONFERENCE__
	//printf("\n   Creating Room\n");
	//CConfRoom room(/*io_service*/);
	SHP_CConfRoom room(new CConfRoom(io_service_));
	//printf("\n   Created Room\n   Reading SDP files\n");
	//using Asio src
	string s1("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=MGCP_server\n""c=IN IP4 10.77.7.19\n" 
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 20062 RTP/AVP 9\n" "b=AS:64");
	string s22("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=MGCP_server2\n" "c=IN IP4 10.77.7.19\n" 
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 11000 RTP/AVP 0\n" "b=AS:64");//////////////97
	string s2("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=Kalimba\n" "c=IN IP4 10.77.7.19\n" 
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 20062 RTP/AVP 0\n" "b=AS:705" "a=rtpmap:97 PCMU/44100/2\n");
	string s3("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=I_ll_Be_Gone\n" "c=IN IP4 10.77.7.19\n"
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 30000 RTP/AVP 0\n" "b=AS:705" "a=rtpmap:97 PCMU/44100/2\n");
	string s4("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=blow_wind_blow\n" "c=IN IP4 10.77.7.19\n"
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 40000 RTP/AVP 0\n" "b=AS:705" /*"a=rtpmap:97 PCMU/44100/2\n"*/
		"a=rtpmap : 18 G729 / 8000" "a=rtpmap : 0 PCMU / 8000");

	IN_OUT inout[] = {
	{ s1, 10000, { strOutAddr, 6000, 6100, 0 },"1" }
	,{ s2, 10002, { strOutAddr, 7000, 7100, 0 },"2" }
	,{ s4, 30000, { strOutAddr, 8000, 8100, 0 },"3" }
	//,{ s2, 40000, { strOutAddr, 9000, 9100, 20 },"4" }
	};
	//printf("\n   Finished reading SDP files\n   Starting giving NewPoint\n");
	for (auto& entry : inout) {
		room->newInitPoint(entry.sdp_string, entry.potIn, entry.rtpdest, entry.CallID);
	}
	//printf("\n   Finished giving NewPoint\n   Starting thread for??\n");
	/*
	FIN_OUT finout[] = {
		
				{ "music.wav", { strOutAddr, 6000, 6100, 20 } }
				, { "telephone_call_from_istanbul.mp3", { strOutAddr, 7000, 7100, 20 } }
				, { "ill_be_gone.mp3", { strOutAddr, 8000, 8100, 20 } }
				, { "kalimba.mp3", { strOutAddr, 9000, 9100, 20 } }
				
		{ s1, { strOutAddr, 6000, 6100, 20 } }
		, { s2, { strOutAddr, 7000, 7100, 20 } }
		, { s3, { strOutAddr, 8000, 8100, 20 } }
		, { s4, { strOutAddr, 9000, 9100, 20 } }
	};
	printf("\n   Finished reading SDP files\n   Starting giving NewPoint to all old dest Points\n");
	for (auto& entry : finout)
		room.newInitPoint(entry.file, entry.rtpdest);
	printf("\n   Finished giving NewPoint\n   Starting thread for??\n");
*/
	/*std::thread th([&](){
				//std::this_thread::sleep_for(chrono::seconds(300));
		io_service.run(); }
	);*///3
	
	printf("\n Set State of room: stActive\n");
	room->setState(CConfRoom::stActive);
	boost::thread_group threads;
	for (int i = 0; i < 3; ++i)
		threads.create_thread(worker_thread);
	/*std::thread th([&](){
		std::this_thread::sleep_for(chrono::seconds(60));
		cout << "\nio_SERVICE RUN";
		io_service.run(); }
	);*/
	/*std::thread th([&](){
		std::this_thread::sleep_for(chrono::seconds(60));
		cout << "\nio_SERVICE RUN";
		io_service.run(); }
	);*/
	/*printf("\nNewDynPoint\n");
	IN_OUT inout1[] = {
		{ s1, 11000, { strOutAddr, 6200, 6300, 0 },"5" }
		, { s4, 21000, { strOutAddr, 7200, 7300, 0 },"6" }
		//,{ s3, 31000, { strOutAddr, 8200, 8300, 0 },"7" }
		,{ s4, 41000, { strOutAddr, 9200, 9300, 20 },"8" }
	};
	for (auto& entry : inout1) {
		//printf("\n   NewPoint %d\n", entry.potIn);
		room.newDynPoint(entry.sdp_string, entry.potIn, entry.rtpdest, entry.CallID);
	}*/
	cout << "\nSLEEPSLEEP";
	boost::this_thread::sleep(boost::posix_time::millisec(10000));
	cout << "\nSLEEPSLEEP";
	threads.join_all();
	//cout << "\n" << room->GetIDPoints();
	//printf("\ndellpoint\n");
	//room->delPoint("2");
	//cout << "\nfk";
	/*room.delPoint("3");*/
	//Sleep(1000000);
	//room.delPoint("1");
//	cout <<"\n fffffff"<< room.GetNumcllPoints()<< "\n";
	//room.setState(CConfRoom::stInactive);
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	//printf("\n   Finished giving NewPoint\n   Starting thread for??\n");
	printf("\nThread should begin join\n");
//	th.join();
	printf("\nThread should have joined\n");

	//////////
	return 0;
#else
	char* outputFile = "output.wav";
	remove(outputFile);

	CDestFusion dest({ strOutAddr, 6000, 6100, 20 });
	CDestFusion dest2({ strOutAddr, 7000, 7100, 20 });
	printf("\nbefore reading sdp\n");
	string s1("v=1\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=MGCP_server\n"
		"c=IN IP4 10.77.7.19\n" "t=0 0\n" "a=tool:libavformat 57.3.100\n"
		"m=audio 20062 RTP/AVP 0\n" "b=AS:64");
	string s22("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=MGCP_server2\n"
		"c=IN IP4 10.77.7.19\n" "t=0 0\n" "a=tool:libavformat 57.3.100\n"
		"m=audio 11000 RTP/AVP 0\n" "b=AS:64");//////////////97
	string s2("v=0\n" "o=- 0 0 IN IP4 127.0.0.1\n" "s=Kalimba\n"
		"c=IN IP4 10.77.7.19\n" "t=0 0\n" "a=tool:libavformat 57.3.100\n"
		"m=audio 20000 RTP/AVP 0\n" "b=AS:705" "a=rtpmap:97 PCMU/44100/2\n");
	string s3("v=0\n"
		"o=- 0 0 IN IP4 127.0.0.1\n" "s=I_ll_Be_Gone\n" "c=IN IP4 10.77.7.19\n"
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 30000 RTP/AVP 8\n"
		"b=AS:705" "a=rtpmap:97 PCMU/44100/2\n");
	string s4("v=0\n"
		"o=- 0 0 IN IP4 127.0.0.1\n" "s=blow_wind_blow\n" "c=IN IP4 10.77.7.19\n"
		"t=0 0\n" "a=tool:libavformat 57.3.100\n" "m=audio 40000 RTP/AVP 0\n"
		"b=AS:705" /*"a=rtpmap:97 PCMU/44100/2\n"*/"a=rtpmap : 18 G729 / 8000" "a=rtpmap : 0 PCMU / 8000");
	SHP_ISrcFusion arrSrc[] =
	{


		//std::make_shared<CSrcCommon>("music.wav"),
		//std::make_shared<CSrcCommon>("telephone_call_from_istanbul.mp3"),
		//std::make_shared<CSrcCommon>("ill_be_gone.mp3"),
		//std::make_shared<CSrcCommon>("kalimba.mp3")

		//std::make_shared<CSrcCommon>("in1.sdp"),
		//std::make_shared<CSrcCommon>("in2.sdp"),
		//std::make_shared<CSrcCommon>("in3.sdp"),
		//std::make_shared<CSrcCommon>("in4.sdp")

		std::make_shared<CSrcCommon>(s1),
		std::make_shared<CSrcCommon>(s2),
		std::make_shared<CSrcCommon>(s3),
		std::make_shared<CSrcCommon>(s4)

		/*

				std::make_shared<CSrcAsio>("in1.sdp", io_service, 10000),
				std::make_shared<CSrcAsio>("in2.sdp", io_service, 20000),
				std::make_shared<CSrcAsio>("in3.sdp", io_service, 30000),
				std::make_shared<CSrcAsio>("in4.sdp", io_service, 40000)
				*/


	};
	printf("\n after reading sdp\n");
	CSrcCash cash[] = { arrSrc[0], arrSrc[1], arrSrc[2], arrSrc[3] };
	printf("\nbefore giving task to threads\n");
	dest.addSrcRef(cash[0].newEntry());
	dest.addSrcRef(cash[2].newEntry());
	dest.addSrcRef(cash[1].newEntry());

	dest2.addSrcRef(cash[3].newEntry());
	dest2.addSrcRef(cash[1].newEntry());
	dest2.addSrcRef(cash[2].newEntry());
	printf("\nafter giving task to threads\n");

	std::thread thAsioIN([&]{io_service.run(); });
	printf("\nio_servise.run();\n");
	//Dests are in separate threads
	/*
		for (auto& c : cash)
		new std::thread(&CSrcCash::run, &c);
		*/


#if 1
	printf("\nbefore starting threads\n");
	std::thread thDest1(&CDestFusion::run, &dest);
	std::thread thDest2(&CDestFusion::run, &dest2);
	printf("\nthreads started, not finished\n");
	thDest1.join();
	printf("\ndest1 join\n");
	//thDest2.join();
#else

	printf("\ntesting 3 \n");
	//Dests are not threading
	dest.runBegin();
	dest2.runBegin();
	int cnt = 0;
	while (dest.isActive() || dest2.isActive())
	{
		if (cnt++ > 20)
			cnt = 0;
		dest.proceedIO_step();
		dest2.proceedIO_step();
	}
	dest.runEnd();
	dest2.runEnd();
#endif // 1
	printf("\n before thAsioIN.join\n");
	thAsioIN.join();
	printf("\n after thAsioIN.join\n");

	return 0;

#pragma region AMIX_origin
	int err;
	printf("\ntesting 4 \n");
	char* audio1Path = "in1.sdp";
	if (open_input_file(audio1Path, &input_format_context_0, &input_codec_context_0) < 0){
		av_log(NULL, AV_LOG_ERROR, "Error while opening file 1\n");
		exit(1);
	}
	//return nextдима
	//av_dump_format(input_format_context_0, 0, audio1Path, 0);

	char* audio2Path = "in2.sdp";

	if (open_input_file(audio2Path, &input_format_context_1, &input_codec_context_1) < 0){
		av_log(NULL, AV_LOG_ERROR, "Error while opening file 2\n");
		exit(1);
	}
	//return next дима
	//av_dump_format(input_format_context_1, 0, audio2Path, 0);



	/* Set up the filtergraph. */
	err = init_filter_graph(&graph, &src0, &src1, &sink);
	printf("Init err = %d\n", err);


	av_log(NULL, AV_LOG_INFO, "Output file : %s\n", outputFile);

	err = open_output_file(outputFile, input_codec_context_0, &output_format_context, &output_codec_context);
	printf("open output file err : %d\n", err);
	//return next дима
	//av_dump_format(output_format_context, 0, outputFile, 1);

	if (write_output_file_header(output_format_context) < 0){
		av_log(NULL, AV_LOG_ERROR, "Error while writing header outputfile\n");
		exit(1);
	}

	process_all();

	if (write_output_file_trailer(output_format_context) < 0){
		av_log(NULL, AV_LOG_ERROR, "Error while writing header outputfile\n");
		exit(1);
	}

	printf("FINISHED\n");

	return 0;
#pragma endregion AMIX_origin
	printf("\ntesting 5 \n");
#endif // AMIX
	printf("\ntesting 6 \n");
}

void test_threading()
{
	CThreadedCircular<shared_ptr<int>> c(11);

	std::thread t2([&]{  
		//while (!c.empty() || !c.isTerminated())
		while (1)
		{
			shared_ptr<int> val; ;
			if (c.try_pop(val))
				cout << *val;
			else
				cout << "NULL";
			cout << "\tremain " << c.size() << '\n';
		}
	});
	std::thread t1([&]{
		for (int i = 0; i < 200; ++i)
		{
			std::this_thread::sleep_for(chrono::microseconds(5));
			c.push(std::make_shared<int>(i));
			
		}
	});
	t1.join();
	//	c.terminate();
	t2.join();
}

