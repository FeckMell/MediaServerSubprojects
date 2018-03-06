#pragma once
#include "stdafx.h"
#include "RTPReceive.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
struct SdpOpaque
{
	using Vector = std::vector<uint8_t>; Vector data; Vector::iterator pos;
};
int sdp_read(void *opaque, uint8_t *buf, int size);
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int CRTPReceive::FirstInit()
{
	av_log_set_level(AV_LOG_VERBOSE);
	av_register_all();
	//avcodec_register_all();
	avfilter_register_all();
	avformat_network_init();
	for (int i = 0; i < tracks; ++i)
	{
		AVFormatContext *input_format_context = NULL;
		AVCodecContext *input_codec_context = NULL;
		SSource a;

		for (int j = 0; j < tracks - 1; ++j)
		{
			AVFilterContext* src = NULL;
			a.src.push_back(src);
		}

		ifcx.push_back(input_format_context);
		iccx.push_back(input_codec_context);
		//out_ifcx.push_back(output_format_context);
		//out_iccx.push_back(output_codec_context);
		afcx.push_back(a);
	}
	return 1;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int CRTPReceive::init(vector<string> SDP)
{
	int err;
	vector<char*> audioPath;
	vector<char*> outputFile;
	vector<string> audioPathStr;

	audioPath.push_back("in2.sdp");
	audioPath.push_back("in3.sdp");
	audioPath.push_back("in4.sdp");
	string temp;
	temp = "rtp://10.77.7.19:29000?localport=0";
	audioPathStr.push_back(temp);
	temp = "rtp://10.77.7.19:29004?localport=0";
	audioPathStr.push_back(temp);
	temp = "rtp://10.77.7.19:29008?localport=0";
	audioPathStr.push_back(temp);

	for (int i = 0; i < tracks; ++i)
	{
		if (open_input_file(SDP[i], &ifcx[i], &iccx[i]) < 0){
			av_log(NULL, AV_LOG_ERROR, "Error while opening file 1\n");
			system("pause");
			exit(3);
		}
		av_dump_format(ifcx[i], 0, /*audioPathStr*/SDP[i].c_str(), 0);
		av_dump_format(ifcx[i], 0, "format.txt", 0);
	}
	for (int i = 0; i < tracks; ++i)
	{
		err = init_filter_graph(i);
		if (err < 0)
		{
			std::printf("Init err = %d\n", err);
		}
	}
	for (int i = 0; i < tracks; ++i)
	{
		AVFormatContext *output_format_context = NULL;
		AVCodecContext *output_codec_context = NULL;
		out_ifcx.push_back(output_format_context);
		out_iccx.push_back(output_codec_context);
		char out[100];
		snprintf(out, sizeof(out), "output%d.wav", i);
		//snprintf(out, sizeof(out), "rtp://10.77.7.19:%d?localport=0", 40000 - i*10000 + i*2);
		outputFile.push_back(out);

		remove(outputFile[i]);
		//av_log(NULL, AV_LOG_INFO, "Output file : %s\n", outputFile[i]);
		err = open_output_file(outputFile[i], i);
		std::printf("open output file err : %d\n", err);
		//av_dump_format(out_ifcx[i], 0, outputFile[i], 1);
		cout << "\n header";
		if (write_output_file_header(out_ifcx[i]) < 0){
			av_log(NULL, AV_LOG_ERROR, "Error while writing header outputfile\n");
			system("pause");
			exit(5);
		}

	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char *const get_error_text(const int error)
{
	static char error_buffer[255];
	av_strerror(error, error_buffer, sizeof(error_buffer));
	return error_buffer;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int CRTPReceive::sdp_open(AVFormatContext **pctx, const char *data, AVDictionary **options) /*noexcept*/
{
	assert(pctx);
	*pctx = avformat_alloc_context();
	assert(*pctx);

	const size_t avioBufferSize = 4096;
	auto avioBuffer = static_cast<uint8_t*>(av_malloc(avioBufferSize));
	auto opaque = new SdpOpaque();
	opaque->data = SdpOpaque::Vector(data, data + strlen(data));
	opaque->pos = opaque->data.begin();

	auto pbctx = avio_alloc_context(avioBuffer, avioBufferSize, 0, opaque, &sdp_read, nullptr, nullptr);
	assert(pbctx);
	(*pctx)->pb = pbctx;

	auto infmt = av_find_input_format("sdp");

	return avformat_open_input(pctx, "memory.sdp", /*nullptr*/infmt, nullptr/*options*/);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int CRTPReceive::init_filter_graph(int ForClient)
{
	AVFilterGraph   *filter_graph;
	AVFilterContext *mix_ctx;
	AVFilter        *mix_filter;
	AVFilterContext *abuffersink_ctx;
	AVFilter        *abuffersink;

	char args[512];
	int err;

	/* Create a new filtergraph, which will contain all the filters. */
	filter_graph = avfilter_graph_alloc();
	if (!filter_graph) {
		av_log(NULL, AV_LOG_ERROR, "Unable to create filter graph.\n");
		return AVERROR(ENOMEM);
	}
	//std::cout << "\n 1";
	/****** abuffer [ForClient][i] ********/
	for (int i = 0; i < tracks; ++i)
	{
		/*for i==ForClient we dont set buffer*/
		if (i == ForClient) continue;
		AVFilter *abuffer0;
		char arg[10];

		/* Create the abuffer filter; * it will be used for feeding the data into the graph. */
		abuffer0 = avfilter_get_by_name("abuffer");
		if (!abuffer0) 
		{
			av_log(NULL, AV_LOG_ERROR, "Could not find the abuffer filter.\n");
			return AVERROR_FILTER_NOT_FOUND;
		}

		/* buffer audio source: the decoded frames from the decoder will be inserted here. */
		if (!iccx[i]->channel_layout)
		{
			iccx[i]->channel_layout = av_get_default_channel_layout(iccx[i]->channels);
		}
			
		snprintf(args, sizeof(args), "sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
			iccx[i]->sample_rate, av_get_sample_fmt_name(iccx[i]->sample_fmt), iccx[i]->channel_layout);
		snprintf(arg, sizeof(arg), "src%d-%d", ForClient, i);
		//разбиение индекса для SSource.
		if (i < ForClient)
			err = avfilter_graph_create_filter(&afcx[ForClient].src[i], abuffer0, arg, args, NULL, filter_graph);
		else
			err = avfilter_graph_create_filter(&afcx[ForClient].src[i-1], abuffer0, arg, args, NULL, filter_graph);
		if (err < 0) 
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
			return err;
		}
	}
	/****** amix ******* */

	//std::cout << "\n 2-7";
	/****** amix ******* */
	/* Create mix filter. */
	mix_filter = avfilter_get_by_name("amix");
	if (!mix_filter) {
		av_log(NULL, AV_LOG_ERROR, "Could not find the mix filter.\n");
		return AVERROR_FILTER_NOT_FOUND;
	}
	//std::printf("\n 8");
	char arg[10];
	snprintf(arg, sizeof(arg), "amix%d", ForClient);
	snprintf(args, sizeof(args), "inputs=%d", tracks - 1);

	err = avfilter_graph_create_filter(&mix_ctx, mix_filter, arg,
		args, NULL, filter_graph);

	if (err < 0) 
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot create audio amix filter\n");
		return err;
	}
	//std::printf("\n 9");
	/* Finally create the abuffersink filter;
	* it will be used to get the filtered data out of the graph. */
	abuffersink = avfilter_get_by_name("abuffersink");
	if (!abuffersink) 
	{
		av_log(NULL, AV_LOG_ERROR, "Could not find the abuffersink filter.\n");
		return AVERROR_FILTER_NOT_FOUND;
	}
	//std::printf("\n 10");
	snprintf(arg, sizeof(arg), "sink%d", ForClient);
	abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, arg);
	if (!abuffersink_ctx) 
	{
		av_log(NULL, AV_LOG_ERROR, "Could not allocate the abuffersink instance.\n");
		return AVERROR(ENOMEM);
	}
	//std::printf("\n 11");
	/* Same sample fmts as the output file. */
	const enum AVSampleFormat Fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE};
	err = av_opt_set_int_list(abuffersink_ctx, "sample_fmts", Fmts, AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
	//std::printf("\n 12");

	//uint8_t ch_layout[64];
	char ch_layout[64];
	av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, OUTPUT_CHANNELS);
	av_opt_set(abuffersink_ctx, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);
	//std::printf("\n 13");
	if (err < 0) 
	{
		av_log(NULL, AV_LOG_ERROR, "Could set options to the abuffersink instance.\n");
		return err;
	}
	//std::printf("\n 14");
	err = avfilter_init_str(abuffersink_ctx, NULL);
	if (err < 0) 
	{
		av_log(NULL, AV_LOG_ERROR, "Could not initialize the abuffersink instance.\n");
		return err;
	}
	//std::printf("\n 15");

	/* Connect the filters; */
	int indexx = 0;
	for (int i = 0; i < tracks - 1; ++i)
	{
		//if (i == ForClient) continue;
		err = avfilter_link(afcx[ForClient].src[i], 0, mix_ctx, indexx);
		if (err < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Error connecting filters\n");
			return err;
		}
		++indexx;
	}
	err = avfilter_link(mix_ctx, 0, abuffersink_ctx, 0);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error connecting filters\n");
		return err;
	}
	//std::printf("\n 16");
	/* Configure the graph. */
	err = avfilter_graph_config(filter_graph, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error while configuring graph : %s\n", get_error_text(err));
		return err;
	}
	//std::printf("\n 17");
	char* dump = avfilter_graph_dump(filter_graph, NULL);
	av_log(NULL, AV_LOG_ERROR, "Graph :\n%s\n", dump);
	//std::printf("\n 18");
	graphVec.push_back(filter_graph);
	sinkVec.push_back(abuffersink_ctx);
	//graph = filter_graph;
	//sink = abuffersink_ctx;
	//std::printf("\n 19");
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Open an input file and the required decoder. */
int CRTPReceive::open_input_file(string sdp_string, AVFormatContext **input_format_context, AVCodecContext **input_codec_context)
{
	AVCodec *input_codec;
	int error;
	//string sdp_string;
	/** Open the input file to read from it. */
	error = sdp_open(input_format_context, sdp_string.c_str(), nullptr);
	if (error < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Could not open input file '%s' (error '%s')\n",
			sdp_string.c_str(), get_error_text(error));
		*input_format_context = NULL;
		return error;
	}
	/*if ((error = avformat_open_input(input_format_context, filename, NULL,
		NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open input file '%s' (error '%s')\n",
			filename, get_error_text(error));
		*input_format_context = NULL;
		return error;
	}*/
	
	/** Get information on the input file (number of streams etc.). */
	if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) 
	{
		av_log(NULL, AV_LOG_ERROR, "Could not open find stream info (error '%s')\n",
			get_error_text(error));
		avformat_close_input(input_format_context);
		return error;
	}

	/** Make sure that there is only one stream in the input file. */
	if ((*input_format_context)->nb_streams != 1) 
	{
		av_log(NULL, AV_LOG_ERROR, "Expected one audio input stream, but found %d\n",
			(*input_format_context)->nb_streams);
		avformat_close_input(input_format_context);
		return AVERROR_EXIT;
	}

	/** Find a decoder for the audio stream. */
	if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[0]->codec->codec_id))) {
		av_log(NULL, AV_LOG_ERROR, "Could not find input codec\n");
		avformat_close_input(input_format_context);
		return AVERROR_EXIT;
	}

	/** Open the decoder for the audio stream to use it later. */
	if ((error = avcodec_open2((*input_format_context)->streams[0]->codec,
		input_codec, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open input codec (error '%s')\n",
			get_error_text(error));
		avformat_close_input(input_format_context);
		return error;
	}

	/** Save the decoder context for easier access later. */
	*input_codec_context = (*input_format_context)->streams[0]->codec;

	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/**
* Open an output file and the required encoder.
* Also set some basic encoder parameters.
* Some of these parameters are based on the input file's parameters.
*/
int CRTPReceive::open_output_file(const char *filename, int i)
{
	AVIOContext *output_io_context = NULL;
	AVStream *stream = NULL;
	AVCodec *output_codec = NULL;
	int error;

	/** Open the output file to write to it. */
	if ((error = avio_open(&output_io_context, filename,
		AVIO_FLAG_WRITE)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s' (error '%s')\n",
			filename, get_error_text(error));
		return error;
	}

	/** Create a new format context for the output container format. */
	if (!(out_ifcx[i] = avformat_alloc_context())) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate output format context\n");
		return AVERROR(ENOMEM);
	}

	/** Associate the output file (pointer) with the container format context. */
	(out_ifcx[i])->pb = output_io_context;

	/** Guess the desired container format based on the file extension. */
	if (!((out_ifcx[i])->oformat = av_guess_format(NULL, filename,
		NULL))) {
		av_log(NULL, AV_LOG_ERROR, "Could not find output file format\n");
		goto cleanup;
	}

	av_strlcpy((out_ifcx[i])->filename, filename,
		sizeof((out_ifcx[i])->filename));

	/** Find the encoder to be used by its name. */
	if (!(output_codec = avcodec_find_encoder((ifcx[i])->streams[0]->codec->codec_id))) {
		av_log(NULL, AV_LOG_ERROR, "Could not find an PCM encoder.\n");
		goto cleanup;
	}

	/** Create a new audio stream in the output file container. */
	if (!(stream = avformat_new_stream(out_ifcx[i], output_codec))) {
		av_log(NULL, AV_LOG_ERROR, "Could not create new stream\n");
		error = AVERROR(ENOMEM);
		goto cleanup;
	}

	/** Save the encoder context for easiert access later. */
	out_iccx[i] = stream->codec;

	/**
	* Set the basic encoder parameters.
	*/
	(out_iccx[i])->channels = OUTPUT_CHANNELS;
	(out_iccx[i])->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);
	(out_iccx[i])->sample_rate = iccx[i]->sample_rate;
	(out_iccx[i])->sample_fmt = AV_SAMPLE_FMT_S16;
	//(*output_codec_context)->bit_rate       = input_codec_context->bit_rate;

	av_log(NULL, AV_LOG_INFO, "output bitrate %d\n", (out_iccx[i])->bit_rate);

	/**
	* Some container formats (like MP4) require global headers to be present
	* Mark the encoder so that it behaves accordingly.
	*/
	if ((out_ifcx[i])->oformat->flags & AVFMT_GLOBALHEADER)
		(out_iccx[i])->flags |= CODEC_FLAG_GLOBAL_HEADER;

	/** Open the encoder for the audio stream to use it later. */
	if ((error = avcodec_open2(out_iccx[i], output_codec, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open output codec (error '%s')\n",
			get_error_text(error));
		goto cleanup;
	}

	return 0;

cleanup:
	avio_close((out_ifcx[i])->pb);
	avformat_free_context(out_ifcx[i]);
	out_ifcx[i] = NULL;
	return error < 0 ? error : AVERROR_EXIT;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Initialize one audio frame for reading from the input file */
int CRTPReceive::init_input_frame(AVFrame **frame)
{
	if (!(*frame = av_frame_alloc())) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate input frame\n");
		return AVERROR(ENOMEM);
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Initialize one data packet for reading or writing. */
void CRTPReceive::init_packet(AVPacket *packet)
{
	av_init_packet(packet);
	/** Set the packet data and size so that it is recognized as being empty. */
	packet->data = NULL;
	packet->size = 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Decode one audio frame from the input file. */
int CRTPReceive::decode_audio_frame(AVFrame *frame, AVFormatContext *input_format_context,
	AVCodecContext *input_codec_context, int *data_present, int *finished)
{
	/** Packet used for temporary storage. */
	AVPacket input_packet;
	int error;
	init_packet(&input_packet);

	/** Read one audio frame from the input file into a temporary packet. */
	if ((error = av_read_frame(input_format_context, &input_packet)) < 0) {
		/** If we are the the end of the file, flush the decoder below. */
		if (error == AVERROR_EOF)
			*finished = 1;
		else {
			av_log(NULL, AV_LOG_ERROR, "Could not read frame (error '%s')\n",
				get_error_text(error));
			return error;
		}
	}

	/**
	* Decode the audio frame stored in the temporary packet.
	* The input audio stream decoder is used to do this.
	* If we are at the end of the file, pass an empty packet to the decoder
	* to flush it.
	*/
	if ((error = avcodec_decode_audio4(input_codec_context, frame,
		data_present, &input_packet)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not decode frame (error '%s')\n",
			get_error_text(error));
		av_free_packet(&input_packet);
		return error;
	}

	/**
	* If the decoder has not been flushed completely, we are not finished,
	* so that this function has to be called again.
	*/
	if (*finished && *data_present)
		*finished = 0;
	av_free_packet(&input_packet);
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Encode one frame worth of audio to the output file. */
int CRTPReceive::encode_audio_frame(AVFrame *frame, AVFormatContext *output_format_context,
	AVCodecContext *output_codec_context, int *data_present)
{
	/** Packet used for temporary storage. */
	AVPacket output_packet;
	int error;
	init_packet(&output_packet);

	/**
	* Encode the audio frame and store it in the temporary packet.
	* The output audio stream encoder is used to do this.
	*/
	if ((error = avcodec_encode_audio2(output_codec_context, &output_packet,
		frame, data_present)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not encode frame (error '%s')\n",
			get_error_text(error));
		av_free_packet(&output_packet);
		return error;
	}

	/** Write one audio frame from the temporary packet to the output file. */
	if (*data_present) {
		if ((error = av_write_frame(output_format_context, &output_packet)) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Could not write frame (error '%s')\n",
				get_error_text(error));
			av_free_packet(&output_packet);
			return error;
		}

		av_free_packet(&output_packet);
	}

	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int CRTPReceive::process_all()
{
	int ret = 0;
	int data_present = 0;
	int finished = 0;
	int total_out_samples = 0;
	int nb_finished = 0;

	vector<int> input_finished;
	vector<int> input_to_read;
	vector<int> total_samples;
	for (int i = 0; i < tracks-1; ++i)
	{
		input_finished.push_back(0);
		input_to_read.push_back(1);
		total_samples.push_back(0);
	}

	std::cout << "\n 1";

	while (nb_finished < tracks) 
	{
		//std::cout << "\n 2";
		int data_present_in_graph = 0;

		for (int i = 0; i < tracks; ++i)
		{
			//cout << "\ni=" << i;
			//if (input_finished[i] || input_to_read[i] == 0) continue;
			input_to_read[i] = 0;
			AVFrame *frame = NULL;

			if (init_input_frame(&frame) > 0) { std::cout << "\n 2.1"; goto end; }

			/** Decode one frame worth of audio samples. */
			if ((ret = decode_audio_frame(frame, ifcx[i], iccx[i], &data_present, &finished)))
			{
				std::cout << "\n 2.2";
				goto end;
			}
			/**
			* If we are at the end of the file and there are no more samples
			* in the decoder which are delayed, we are actually finished.
			* This must not be treated as an error.
			*/
			if (finished && !data_present) 
			{
				input_finished[i] = 1;
				nb_finished++;
				ret = 0;
				av_log(NULL, AV_LOG_INFO, "Input n°%d finished. Write NULL frame \n", i);
				//каждому клиенту в соответствующий буффер даем фрейм
				for (int j = 0; j < tracks; ++j)
				{
					//std::cout << "\ni= "<<i<<" j= "<<j<<"\n";
					if (i == j) continue;
					if (j < i)
						ret = av_buffersrc_write_frame(afcx[j].src[i-1], NULL);
					else 
						ret = av_buffersrc_write_frame(afcx[j].src[i], NULL);
					if (ret < 0)
					{
						av_log(NULL, AV_LOG_ERROR, "Error writing EOF null frame for input %d\n", i);
						std::cout << "\n 2.3";
						goto end;
					}
				}
					
			}
			else if (data_present) 
			{ 
				/** If there is decoded data, convert and store it */
				/* push the audio data from decoded frame into the filtergraph */
				//каждому клиенту в соответствующий буффер даем фрейм
				for (int j = 0; j < tracks; ++j)
				{
					//std::cout << "\ni= " << i << " j= " << j << "\n";
					if (i == j) continue;

					if (j < i)
						ret = av_buffersrc_write_frame(afcx[j].src[i - 1], frame);
					else
						ret = av_buffersrc_write_frame(afcx[j].src[i], frame);
					if (ret < 0)
					{
						av_log(NULL, AV_LOG_ERROR, "Error writing EOF null frame for input %d\n", i);
						std::cout << "\n 2.3";
						goto end;
					}
				}

				/*av_log(NULL, AV_LOG_INFO, "add %d samples on input %d (%d Hz, time=%f, ttime=%f)\n",
					frame->nb_samples, i, iccx[i]->sample_rate,
					(double)frame->nb_samples / iccx[i]->sample_rate,
					(double)(total_samples[i] += frame->nb_samples) / iccx[i]->sample_rate);*/

			}

			av_frame_free(&frame);

			data_present_in_graph = data_present | data_present_in_graph;
		}
		//std::cout << "\n 3";
		if (data_present_in_graph)
		{
			/* pull filtered audio from the filtergraph */
			for (int k=0; k < tracks; ++k)
			{
				AVFrame *filt_frame = av_frame_alloc();
				while (1)
				{
					ret = av_buffersink_get_frame(sinkVec[k], filt_frame);
					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					{
						for (int i = 0; i < tracks - 1; i++)
						{
							if (av_buffersrc_get_nb_failed_requests(afcx[k].src[i]) > 0)
							{
								input_to_read[i] = 1;
								/*av_log(NULL, AV_LOG_INFO, "Need to read input %d\n", i);*/
							}
						}

						break;
					}
					if (ret < 0){ std::cout << "\n 2.5"; goto end; }

					/*av_log(NULL, AV_LOG_INFO, "remove %d samples from sink%d (%d Hz, time=%f, ttime=%f)\n",
						filt_frame->nb_samples, k, out_iccx[k]->sample_rate,
						(double)filt_frame->nb_samples / out_iccx[k]->sample_rate,
						(double)(total_out_samples += filt_frame->nb_samples) / out_iccx[k]->sample_rate);*/

					//av_log(NULL, AV_LOG_INFO, "Data read from graph\n");
					//for (int j = 0; j < tracks; ++j)
					//{
					ret = encode_audio_frame(filt_frame, out_ifcx[k], out_iccx[k], &data_present);
					if (ret < 0){ std::cout << "\n 2.6"; goto end; }
					//}
					av_frame_unref(filt_frame);
				}/**/
				av_frame_free(&filt_frame);
			}/*****/			
		}
		else 
		{
			av_log(NULL, AV_LOG_INFO, "No data in graph\n");
			for (int i = 0; i < tracks; i++)
			{
				input_to_read[i] = 1;
			}
		}
	}
	return 0;

end:

	if (ret < 0 && ret != AVERROR_EOF) {
		printf("av_log(NULL, AV_LOG_ERROR, Error occurred : %s\n, av_err2str(ret));");
		//av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));
		system("pause");
		exit(1);
	}
	system("pause");
	exit(2);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Write the header of the output file container. */
int CRTPReceive::write_output_file_header(AVFormatContext *output_format_context)
{
	int error;
	if ((error = avformat_write_header(output_format_context, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not write output file header (error '%s')\n",
			get_error_text(error));
		return error;
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/** Write the trailer of the output file container. */
int CRTPReceive::write_output_file_trailer(AVFormatContext *output_format_context)
{
	int error;
	if ((error = av_write_trailer(output_format_context)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not write output file trailer (error '%s')\n",
			get_error_text(error));
		return error;
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int sdp_read(void *opaque, uint8_t *buf, int size) /*noexcept*/
{
	assert(opaque);
	assert(buf);
	auto octx = static_cast<SdpOpaque*>(opaque);

	if (octx->pos == octx->data.end()) {
		return 0;
	}

	auto dist = static_cast<int>(std::distance(octx->pos, octx->data.end()));
	auto count = std::min(size, dist);
	std::copy(octx->pos, octx->pos + count, buf);
	octx->pos += count;

	return count;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------