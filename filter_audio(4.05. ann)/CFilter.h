#pragma once
#include "stdafx.h"
//#include "RTPReceive.h"
#ifdef Filter
#define OUTPUT_CHANNELS 2

class CFilter
{
public:
	CFilter(SHP_CRTPReceive Sources_, int a, int b) { Sources = Sources_; initFilter(a, b); }
	void initFilter(int a, int b);
	int init_filter_graph();
	int process_all();
	int init_input_frame(AVFrame **frame);
	int decode_audio_frame(AVFrame *frame,
		AVFormatContext *input_format_context,
		AVCodecContext *input_codec_context,
		int *data_present, int *finished);
	int encode_audio_frame(AVFrame *frame,
		AVFormatContext *output_format_context,
		AVCodecContext *output_codec_context,
		int *data_present);
	void init_packet(AVPacket *packet);


	char *const get_error_text(const int error);

private:
	AVFormatContext *output_format_context = NULL;
	AVCodecContext *output_codec_context = NULL;

	AVFilterGraph *graph = NULL;
	AVFilterContext  *sink = NULL;

	vector<AVFilterContext *> afcx;
	vector<AVFormatContext *> ifcx;
	vector<AVCodecContext *> iccx;
	SHP_CRTPReceive Sources;
};

typedef std::shared_ptr<CFilter> SHP_CFilter;
#endif