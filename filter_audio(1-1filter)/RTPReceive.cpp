// FFMPEGtest2.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "RTPReceive.h"

int CRTPReceive::FirstInit()
{
	av_log_set_level(AV_LOG_VERBOSE);
	av_register_all();
	avfilter_register_all();
	avformat_network_init();
	for (int i = 0; i < tracks; ++i)
	{
		AVFormatContext *input_format_context = NULL;
		AVCodecContext *input_codec_context = NULL;
		AVFilterContext* src = NULL;
		ifcx.push_back(input_format_context);
		iccx.push_back(input_codec_context);
		afcx.push_back(src);
		//framebuffers.push_back(circular);
	}
	return 1;
}
int CRTPReceive::init()
{
	
	int err;

	vector<char*> audioPath;

	audioPath.push_back("in2.sdp");
	audioPath.push_back("in3.sdp");
	audioPath.push_back("in4.sdp");

	char* outputFile = "output.wav";
	char* outputFile2 = "output2.wav";

	for (int i = 0; i < tracks; ++i)
	{
		if (open_input_file(audioPath[i], &ifcx[i], &iccx[i]) < 0){
			av_log(NULL, AV_LOG_ERROR, "Error while opening file 1\n");
			exit(3);
		}
		av_dump_format(ifcx[i], 0, audioPath[i], 0);
	}

	err = init_filter_graph(/*&graph, &src0, &src1, &sink*/);
	printf("Init err = %d\n", err);

	//AVFormatContext *output_format_context = NULL;
	//AVCodecContext *output_codec_context = NULL;
	remove(outputFile);

	av_log(NULL, AV_LOG_INFO, "Output file : %s\n", outputFile);

	err = open_output_file(outputFile, iccx[0], &output_format_context, &output_codec_context);
	printf("open output file err : %d\n", err);
	av_dump_format(output_format_context, 0, outputFile, 1);

	if (write_output_file_header(output_format_context) < 0){
		av_log(NULL, AV_LOG_ERROR, "Error while writing header outputfile\n");
		exit(5);
	}
	//out_ifcx.push_back(output_format_context);
	//out_iccx.push_back(output_codec_context);

	return 0;
}

char *const get_error_text(const int error)
{
	static char error_buffer[255];
	av_strerror(error, error_buffer, sizeof(error_buffer));
	return error_buffer;
}

int CRTPReceive::init_filter_graph(/*AVFilterGraph **graph, AVFilterContext **src0, AVFilterContext **src1,
	AVFilterContext **sink*/)
{
	AVFilterGraph *filter_graph;
	//AVFilter        *abuffer1;
	//AVFilter        *abuffer0;
	AVFilterContext *mix_ctx;
	AVFilter        *mix_filter;
	AVFilterContext *abuffersink_ctx;
	AVFilter        *abuffersink;

	char args[512];

	int err;
	printf("\n 1");
	/* Create a new filtergraph, which will contain all the filters. */
	filter_graph = avfilter_graph_alloc();
	if (!filter_graph) {
		av_log(NULL, AV_LOG_ERROR, "Unable to create filter graph.\n");
		return AVERROR(ENOMEM);
	}
	printf("\n 2");
	for (int i = 0; i < tracks; ++i)
	{
		/****** abuffer 0 ********/
		AVFilter        *abuffer0;
		/* Create the abuffer filter;
		* it will be used for feeding the data into the graph. */
		abuffer0 = avfilter_get_by_name("abuffer");
		if (!abuffer0) {
			av_log(NULL, AV_LOG_ERROR, "Could not find the abuffer filter.\n");
			return AVERROR_FILTER_NOT_FOUND;
		}
		printf("\n 3");
		/* buffer audio source: the decoded frames from the decoder will be inserted here. */
		if (!iccx[i]->channel_layout)
			iccx[i]->channel_layout = av_get_default_channel_layout(iccx[i]->channels);
		snprintf(args, sizeof(args),
			"sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
			iccx[i]->sample_rate,
			av_get_sample_fmt_name(iccx[i]->sample_fmt), iccx[i]->channel_layout);

		printf("\n 4");
		char arg[10];
		snprintf(arg, sizeof(arg), "src%d", i);
		err = avfilter_graph_create_filter(&afcx[i], abuffer0, arg, args, NULL, filter_graph);
		if (err < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
			return err;
		}
		printf("\n 5");
	}

	/****** amix ******* */
	/* Create mix filter. */
	mix_filter = avfilter_get_by_name("amix");
	if (!mix_filter) {
		av_log(NULL, AV_LOG_ERROR, "Could not find the mix filter.\n");
		return AVERROR_FILTER_NOT_FOUND;
	}
	printf("\n 8");
	snprintf(args, sizeof(args), "inputs=3");

	err = avfilter_graph_create_filter(&mix_ctx, mix_filter, "amix",
		args, NULL, filter_graph);

	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create audio amix filter\n");
		return err;
	}
	printf("\n 9");
	/* Finally create the abuffersink filter;
	* it will be used to get the filtered data out of the graph. */
	abuffersink = avfilter_get_by_name("abuffersink");
	if (!abuffersink) {
		av_log(NULL, AV_LOG_ERROR, "Could not find the abuffersink filter.\n");
		return AVERROR_FILTER_NOT_FOUND;
	}
	printf("\n 10");
	abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");
	if (!abuffersink_ctx) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate the abuffersink instance.\n");
		return AVERROR(ENOMEM);
	}
	printf("\n 11");
	/* Same sample fmts as the output file. */
	const enum AVSampleFormat
		Fmts[] = {
		AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE
	};
	err = av_opt_set_int_list(abuffersink_ctx, "sample_fmts",
		Fmts,
		AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
	printf("\n 12");

	//uint8_t ch_layout[64];
	char ch_layout[64];
	av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, OUTPUT_CHANNELS);
	av_opt_set(abuffersink_ctx, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);
	printf("\n 13");
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could set options to the abuffersink instance.\n");
		return err;
	}
	printf("\n 14");
	err = avfilter_init_str(abuffersink_ctx, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not initialize the abuffersink instance.\n");
		return err;
	}
	printf("\n 15");

	/* Connect the filters; */
	int indexx = 0;
	/* Connect the filters; */

	for (auto& entry : afcx)
	{
		err = avfilter_link(entry, 0, mix_ctx, indexx);
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
	printf("\n 16");
	/* Configure the graph. */
	err = avfilter_graph_config(filter_graph, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error while configuring graph : %s\n", get_error_text(err));
		return err;
	}
	printf("\n 17");
	char* dump = avfilter_graph_dump(filter_graph, NULL);
	av_log(NULL, AV_LOG_ERROR, "Graph :\n%s\n", dump);
	printf("\n 18");

	graph = filter_graph;
	sink = abuffersink_ctx;
	printf("\n 19");
	return 0;
}


/** Open an input file and the required decoder. */
int CRTPReceive::open_input_file(const char *filename,
	AVFormatContext **input_format_context,
	AVCodecContext **input_codec_context)
{
	AVCodec *input_codec;
	int error;

	/** Open the input file to read from it. */
	if ((error = avformat_open_input(input_format_context, filename, NULL,
		NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open input file '%s' (error '%s')\n",
			filename, get_error_text(error));
		*input_format_context = NULL;
		return error;
	}

	/** Get information on the input file (number of streams etc.). */
	if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open find stream info (error '%s')\n",
			get_error_text(error));
		avformat_close_input(input_format_context);
		return error;
	}

	/** Make sure that there is only one stream in the input file. */
	if ((*input_format_context)->nb_streams != 1) {
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

/**
* Open an output file and the required encoder.
* Also set some basic encoder parameters.
* Some of these parameters are based on the input file's parameters.
*/
int CRTPReceive::open_output_file(const char *filename,
	AVCodecContext *input_codec_context,
	AVFormatContext **output_format_context,
	AVCodecContext **output_codec_context)
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
	if (!(*output_format_context = avformat_alloc_context())) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate output format context\n");
		return AVERROR(ENOMEM);
	}

	/** Associate the output file (pointer) with the container format context. */
	(*output_format_context)->pb = output_io_context;

	/** Guess the desired container format based on the file extension. */
	if (!((*output_format_context)->oformat = av_guess_format(NULL, filename,
		NULL))) {
		av_log(NULL, AV_LOG_ERROR, "Could not find output file format\n");
		goto cleanup;
	}

	av_strlcpy((*output_format_context)->filename, filename,
		sizeof((*output_format_context)->filename));

	/** Find the encoder to be used by its name. */
	if (!(output_codec = avcodec_find_encoder(AV_CODEC_ID_PCM_S16LE))) {
		av_log(NULL, AV_LOG_ERROR, "Could not find an PCM encoder.\n");
		goto cleanup;
	}

	/** Create a new audio stream in the output file container. */
	if (!(stream = avformat_new_stream(*output_format_context, output_codec))) {
		av_log(NULL, AV_LOG_ERROR, "Could not create new stream\n");
		error = AVERROR(ENOMEM);
		goto cleanup;
	}

	/** Save the encoder context for easiert access later. */
	*output_codec_context = stream->codec;

	/**
	* Set the basic encoder parameters.
	*/
	(*output_codec_context)->channels = OUTPUT_CHANNELS;
	(*output_codec_context)->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);
	(*output_codec_context)->sample_rate = input_codec_context->sample_rate;
	(*output_codec_context)->sample_fmt = AV_SAMPLE_FMT_S16;
	//(*output_codec_context)->bit_rate       = input_codec_context->bit_rate;

	av_log(NULL, AV_LOG_INFO, "output bitrate %d\n", (*output_codec_context)->bit_rate);

	/**
	* Some container formats (like MP4) require global headers to be present
	* Mark the encoder so that it behaves accordingly.
	*/
	if ((*output_format_context)->oformat->flags & AVFMT_GLOBALHEADER)
		(*output_codec_context)->flags |= CODEC_FLAG_GLOBAL_HEADER;

	/** Open the encoder for the audio stream to use it later. */
	if ((error = avcodec_open2(*output_codec_context, output_codec, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open output codec (error '%s')\n",
			get_error_text(error));
		goto cleanup;
	}

	return 0;

cleanup:
	avio_close((*output_format_context)->pb);
	avformat_free_context(*output_format_context);
	*output_format_context = NULL;
	return error < 0 ? error : AVERROR_EXIT;
}

/** Initialize one audio frame for reading from the input file */
int CRTPReceive::init_input_frame(AVFrame **frame)
{
	if (!(*frame = av_frame_alloc())) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate input frame\n");
		return AVERROR(ENOMEM);
	}
	return 0;
}

/** Initialize one data packet for reading or writing. */
void CRTPReceive::init_packet(AVPacket *packet)
{
	av_init_packet(packet);
	/** Set the packet data and size so that it is recognized as being empty. */
	packet->data = NULL;
	packet->size = 0;
}


/** Decode one audio frame from the input file. */
int CRTPReceive::decode_audio_frame(AVFrame *frame,
	AVFormatContext *input_format_context,
	AVCodecContext *input_codec_context,
	int *data_present, int *finished)
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

/** Encode one frame worth of audio to the output file. */
int CRTPReceive::encode_audio_frame(AVFrame *frame,
	AVFormatContext *output_format_context,
	AVCodecContext *output_codec_context,
	int *data_present)
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
	for (int i = 0; i < tracks; ++i)
	{
		input_finished.push_back(0);
		input_to_read.push_back(1);
		total_samples.push_back(0);
	}

	std::cout << "\n 1";

	while (nb_finished < tracks) 
	{
		std::cout << "\n 2";
		int data_present_in_graph = 0;

		for (int i = 0; i < tracks; i++)
		{
			if (input_finished[i] || input_to_read[i] == 0){
				continue;
			}

			input_to_read[i] = 0;

			AVFrame *frame = NULL;

			if (init_input_frame(&frame) > 0){
				std::cout << "\n 2.1";
				goto end;
			}

			/** Decode one frame worth of audio samples. */
			if ((ret = decode_audio_frame(frame, ifcx[i], iccx[i], &data_present, &finished))){
				std::cout << "\n 2.2";
				goto end;
			}
			/**
			* If we are at the end of the file and there are no more samples
			* in the decoder which are delayed, we are actually finished.
			* This must not be treated as an error.
			*/
			if (finished && !data_present) {
				input_finished[i] = 1;
				nb_finished++;
				ret = 0;
				av_log(NULL, AV_LOG_INFO, "Input n°%d finished. Write NULL frame \n", i);

				ret = av_buffersrc_write_frame(afcx[i], NULL);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error writing EOF null frame for input %d\n", i);
					std::cout << "\n 2.3";
					goto end;
				}
			}
			else if (data_present) { /** If there is decoded data, convert and store it */
				/* push the audio data from decoded frame into the filtergraph */
				ret = av_buffersrc_write_frame(afcx[i], frame);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error while feeding the audio filtergraph\n");
					std::cout << "\n 2.4";
					goto end;
				}

				av_log(NULL, AV_LOG_INFO, "add %d samples on input %d (%d Hz, time=%f, ttime=%f)\n",
					frame->nb_samples, i, iccx[i]->sample_rate,
					(double)frame->nb_samples / iccx[i]->sample_rate,
					(double)(total_samples[i] += frame->nb_samples) / iccx[i]->sample_rate);

			}

			av_frame_free(&frame);

			data_present_in_graph = data_present | data_present_in_graph;
		}
		std::cout << "\n 3";
		if (data_present_in_graph){
			AVFrame *filt_frame = av_frame_alloc();

			/* pull filtered audio from the filtergraph */
			while (1) {
				ret = av_buffersink_get_frame(sink, filt_frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
					for (int i = 0; i < tracks; i++){
						if (av_buffersrc_get_nb_failed_requests(afcx[i]) > 0){
							input_to_read[i] = 1;
							av_log(NULL, AV_LOG_INFO, "Need to read input %d\n", i);
						}
					}

					break;
				}
				if (ret < 0){ std::cout << "\n 2.5"; goto end; }
					

				av_log(NULL, AV_LOG_INFO, "remove %d samples from sink (%d Hz, time=%f, ttime=%f)\n",
					filt_frame->nb_samples, output_codec_context->sample_rate,
					(double)filt_frame->nb_samples / output_codec_context->sample_rate,
					(double)(total_out_samples += filt_frame->nb_samples) / output_codec_context->sample_rate);

				//av_log(NULL, AV_LOG_INFO, "Data read from graph\n");
				ret = encode_audio_frame(filt_frame, output_format_context, output_codec_context, &data_present);
				if (ret < 0){ std::cout << "\n 2.6"; goto end; }
					

				av_frame_unref(filt_frame);
			}

			av_frame_free(&filt_frame);
		}
		else {
			av_log(NULL, AV_LOG_INFO, "No data in graph\n");
			for (int i = 0; i < tracks; i++){
				input_to_read[i] = 1;
			}
		}

	}

	return 0;

end:
	//    avcodec_close(input_codec_context);
	//    avformat_close_input(&input_format_context);
	//    av_frame_free(&frame);
	//    av_frame_free(&filt_frame);

	if (ret < 0 && ret != AVERROR_EOF) {
		printf("av_log(NULL, AV_LOG_ERROR, Error occurred : %s\n, av_err2str(ret));");
		//av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));
		system("pause");
		exit(1);
	}

	exit(2);
}



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