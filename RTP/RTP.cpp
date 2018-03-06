// RTP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_array.hpp>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
#include "libavutil/opt.h"
}

#include "ffmpeg_rtp.h"
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avcodec.lib")
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avutil.lib")

enum { max_length = 1024 };

namespace asio = boost::asio;
using boost::asio::ip::udp;

enum RTP_PAY_LOADTYPE{ 
//	PT	Encoding Name	A / V	Clock Rate(Hz) 	Channels 	Reference
	PCMU, // 	A 	8000 	1[RFC3551]
	Reserved1,
	Reserved2,
	GSM,// 	A 	8000 	1[RFC3551]
	G723,// 	A 	8000 	1[Vineet_Kumar][RFC3551]
	DVI4_8000,// 	A 	8000 	1[RFC3551]
	DVI4_16000,// 	A 	16000 	1[RFC3551]
	LPC,//  	A 	8000 	1[RFC3551]
	PCMA,//  	A 	8000 	1[RFC3551]
	G722,//  	A 	8000 	1[RFC3551]
	L16_2,//  	A 	44100 	2[RFC3551]
	L16_1,//  	A 	44100 	1[RFC3551]
	QCELP,//  	A 	8000 	1[RFC3551]
	CN,//  	A 	8000 	1[RFC3389]
	MPA,//  	A 	90000[RFC3551][RFC2250]
	G728,//  	A 	8000 	1[RFC3551]
	DVI4_11025,//  	A 	11025 	1[Joseph_Di_Pol]
	DVI4_22050,//  	A 	22050 	1[Joseph_Di_Pol]
	G729,//  	A 	8000 	1[RFC3551]
	Reserved3,//  	A
	Unassigned1,//  	A
	Unassigned2,//  	A
	Unassigned3,//  	A
	Unassigned4,//  	A
	Unassigned5,//  	V
	CelB,//  	V 	90000[RFC2029]
	JPEG,//  	V 	90000[RFC2435]
	Unassigned6,//  	V
	nv,//  	V 	90000[RFC3551]
	Unassigned7,//  	V
	Unassigned8,//  	V
	H261,//  	V 	90000[RFC4587]
	MPV,//  	V 	90000[RFC2250]
	MP2T,//  	AV 	90000[RFC2250]
	H263//  	V 	90000[Chunrong_Zhu]
/*
	35 - 71 	Unassigned ?
	72 - 76 	Reserved for RTCP conflict avoidance[RFC3551]
	77 - 95 	Unassigned ?
	96 - 127 	dynamic ? [RFC3551]*/
};

#pragma pack(push, 1)
/* RTP «ј√ќЋќ¬ќ . 12 байт */
struct TRTP_header{
	/* первый байт */
	uint8_t csrc_len : 4;
	uint8_t extension : 1;
	uint8_t padding : 1;
	uint8_t version : 2;
	/* второй байт */
	uint8_t payload_type : 7;
	uint8_t marker : 1;
	/* третий-четвертый байты */
	uint16_t seq_no;
	/* п€тый-восьмой байты */
	uint32_t timestamp;
	/* дев€тый-двенадцатый байт */
	uint32_t ssrc;
}
//__attribute__((packed)) 
;

struct TRTP_MediaPacket{
	TRTP_header header;
	char data;
};

#pragma pack(pop)


class CRTP_Session
{
public:
	CRTP_Session(boost::asio::io_service& io_service, const udp::endpoint& pnt)
		: sequen(0), socket(io_service, udp::endpoint(udp::v4(), 0)), endpnt(pnt)
	{}
	void sendMediaPacket(boost::shared_array<char> pData, size_t lenData);

private:
	udp::socket		socket; 
	udp::endpoint	endpnt;
	uint16_t		sequen;
};
int payloadtype = 0;
void CRTP_Session::sendMediaPacket(boost::shared_array<char> pData, size_t lenData)
{
	TRTP_MediaPacket* pMediaPacket = reinterpret_cast<TRTP_MediaPacket*>(pData.get());
	TRTP_header* pRtpHdr = &pMediaPacket->header;
	memset(pRtpHdr, 0, sizeof(TRTP_header));
	pRtpHdr->version = 2;
	pRtpHdr->marker = 1;
/*

	pRtpHdr->csrc_len = 0;
	pRtpHdr->extension = 0;
	pRtpHdr->padding = 0;*/
	//pRtpHdr->ssrc = htonl(SSRC_NUM);
	assert(MP2T == 33);
	pRtpHdr->payload_type = payloadtype;// MP2T;
	pRtpHdr->seq_no = htons((sequen++));
	pRtpHdr->timestamp = htonl(GetTickCount());

	auto sz = socket.send_to(boost::asio::buffer(pData.get(), lenData), endpnt);
	printf("Sent\tsz=%lu\tseq=%lu\n", sz, sequen-1);
	Sleep(20);
}

static const char *get_error_text(const int error)
{
	static char error_buffer[255];
	av_strerror(error, error_buffer, sizeof(error_buffer));
	return error_buffer;
}
/** The output bit rate in kbit/s */
#define OUTPUT_BIT_RATE 96000
/** The number of output channels */
#define OUTPUT_CHANNELS 2

AVFormatContext * Create_outRTP_Context(AVStream *strmIN, const udp::endpoint& endpnt)
{
//	AVOutputFormat *rtp_format = av_guess_format("rtp", NULL, NULL);
	int error = 0;

	AVFormatContext *ctxOut	= nullptr;// = avformat_alloc_context();
	AVIOContext *ioOut		= nullptr;
	char filename[1024] = { 0 };// = "rtp://10.77.7.19:8000";
	_snprintf_s(filename, sizeof(filename), sizeof(filename) - 1,
		"rtp://%s:%d", endpnt.address().to_string().c_str(), endpnt.port());
	avformat_alloc_output_context2(&ctxOut, NULL, "rtp", filename);

	AVStream *avs = NULL;
	if (!ctxOut /*|| !rtp_format*/)
		return nullptr;

	if ((error = avio_open(&ctxOut->pb, filename, AVIO_FLAG_WRITE)) < 0) {
		fprintf(stderr, "Could not open output file '%s' (error '%s')\n",
			filename, get_error_text(error));
		return nullptr;
	}
/*
	auto ret = avio_open(&ctxOut->pb, "rtp://localhost:6000", AVIO_FLAG_WRITE);
	if (ret < 0)
		return nullptr;*/
//	!write_audio_frame(avc, &audio_st)

/*
	AVDictionaryEntry *entry = av_dict_get(strmIN->metadata, "title", NULL, 0);
	av_dict_set(&ctxOut->metadata, "title", entry ? entry->value : "No Title", 0);
*/



/*
	_snprintf_s(ctxOut->filename, sizeof(ctxOut->filename), sizeof(ctxOut->filename)-1, 
		"rtp://%s:%d", "127.0.0.1", 6000);*/
	AVCodec *output_codec = nullptr;
	if (!(output_codec = avcodec_find_encoder(strmIN->codec->codec_id))) {
		fprintf(stderr, "Could not find an encoder.\n");
		return nullptr;
	}
	//output_codec->s
	auto strmOut = avformat_new_stream(ctxOut, output_codec);

/*
	error = avcodec_copy_context(strmOut->codec, strmIN->codec);
	if (error) 
		return nullptr;*/


	//avcodec_open2(ctxOut->streams[0]->codec, );
//	avc->nb_streams = 1;

/*
	avc->streams = av_malloc_array(avc->nb_streams, sizeof(*avc->streams));
	avs = av_malloc_array(avc->nb_streams, sizeof(*avs));
	if (!avc->streams || !avs)
		return nullptr;
	avc->streams[0] = &avs[0];
	avc->streams[0]->codec = stIN->codec;
*/

	/** Save the encoder context for easier access later. */
	AVCodecContext *output_codec_context = strmOut->codec;

	/**
	* Set the basic encoder parameters.
	* The input file's sample rate is used to avoid a sample rate conversion.
	*/
	output_codec_context->channels = OUTPUT_CHANNELS;
	output_codec_context->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);
	output_codec_context->sample_rate = strmIN->codec->sample_rate;
	output_codec_context->sample_fmt = output_codec->sample_fmts[0];
	output_codec_context->bit_rate = OUTPUT_BIT_RATE;

	/** Allow the use of the experimental AAC encoder */
/*
	output_codec_context->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
*/

	/** Set the sample rate for the container. */
	strmOut->time_base.den = strmIN->codec->sample_rate;
	strmOut->time_base.num = 1;


	if ((error = avcodec_open2(strmOut->codec, output_codec, NULL)) < 0) {
		fprintf(stderr, "Could not open output codec (error '%s')\n",
			get_error_text(error));
		return nullptr;
	}


	char buffSDP[2048] = { 0 };
	av_sdp_create(&ctxOut, 1, buffSDP, sizeof(buffSDP)-1);
	printf("-------------SDP-------------\n%s\n-----------------------------\n", buffSDP);
	return ctxOut;
}

void streamFileByRTP(asio::io_service& io_service, const udp::endpoint& endpnt, const char* filename)
{
	AVFormatContext *ctxFile = avformat_alloc_context();

	if (avformat_open_input(&ctxFile, filename, NULL, NULL) != 0)
		return;
	av_dump_format(ctxFile, 0, filename, 0);

	int audioIdx = -1;
	for (unsigned i = 0; audioIdx == -1 && i < ctxFile->nb_streams; ++i)
	{
		auto pCodec =  ctxFile->streams[i]->codec;
		if (pCodec->codec_type == /*AVMEDIA_TYPE_VIDEO*/  AVMEDIA_TYPE_AUDIO)
			audioIdx = i;
	}
	if (audioIdx == -1)
		return;

	AVPacket packet;
	AVStream *audio_stream = ctxFile->streams[audioIdx];


	payloadtype = ff_rtp_get_payload_type(ctxFile, audio_stream->codec, audioIdx);
	//rtp_new_connection();
	CRTP_Session rtpSession(io_service, endpnt);

	/////////////////////////////////	
	AVFormatContext* ctxRTP = Create_outRTP_Context(audio_stream, endpnt);
	if (!ctxRTP)
		return;
	int err = 0;
	if ((err = avformat_write_header(ctxRTP, NULL)) < 0) {
		fprintf(stderr, "Could not write output file header (error '%s')\n",
			get_error_text(err));
		return;
	}
	while (av_read_frame(ctxFile, &packet) >= 0)
	{
		if (packet.stream_index == audioIdx)
		{
			av_packet_rescale_ts(&packet,
				ctxRTP->streams[0]->time_base,
				audio_stream->time_base
				);
			err = av_interleaved_write_frame(ctxRTP, &packet);
//			err = av_write_frame(ctxRTP, &packet);
			if (err)
			{
				char errBuff[2048];
				auto e = av_strerror(err, errBuff, sizeof(errBuff)-1);
				auto p = av_make_error_string(errBuff, sizeof(errBuff)-1, err);
				err = 0;
			}
			printf(".");

/*
			const size_t szPacket = sizeof(TRTP_MediaPacket) + packet.buf->size - 1;
			boost::shared_array<char> shpMediaPack(new char[szPacket]);
			TRTP_MediaPacket* pMediaPack = reinterpret_cast<TRTP_MediaPacket*>(shpMediaPack.get());
			memcpy(&pMediaPack->data, packet.buf->data, packet.buf->size);
			rtpSession.sendMediaPacket(shpMediaPack, szPacket);*/
		}
		av_free_packet(&packet);
	}
	printf("\n");

	if ((err = av_write_trailer(ctxRTP)) < 0) {
		fprintf(stderr, "Could not write output file trailer (error '%s')\n",
			get_error_text(err));
	}

	avformat_close_input(&ctxFile);
	avformat_free_context(ctxFile);
}

void initAV()
{
	av_register_all();
	avcodec_register_all();
	avformat_network_init();
	//avnet
}

int main(int argc, char* argv[])
{
	assert(sizeof(TRTP_header) == 12);
	try
	{
		if (argc < 4)
		{
			printf("Usage: %s <filename> <host> <port>\n", argv[0]);
			return 1;
		}
		const char* filename	= argv[1];
		const char*	host		= argv[2];
		const char* port		= argv[3];
		initAV();

		boost::asio::io_service io_service;
		udp::resolver resolver(io_service);
		printf_s("Resolving %s %s...\n", host, port);

		udp::endpoint endpntRTP = *resolver.resolve({ udp::v4(), host, port });
		printf_s("\tdest:\t%s (%s)\n\tport:\t%d\n",
			host, endpntRTP.address().to_string().c_str(), endpntRTP.port());

		streamFileByRTP(io_service, endpntRTP, filename);

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}

