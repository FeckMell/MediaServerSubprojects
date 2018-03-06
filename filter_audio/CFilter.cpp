#pragma once
#include "stdafx.h"
#ifdef Filter
#include "CFilter.h"
//#include "RTPReceive.h"
//-------------------------------------------------------------------------
char *const CFilter::get_error_text(const int error)
{
	static char error_buffer[255];
	av_strerror(error, error_buffer, sizeof(error_buffer));
	return error_buffer;
}
//-------------------------------------------------------------------------
void CFilter::initFilter(int a, int b)
{
	int err;
	/*ifcx.push_back(Sources->getFormatContext(a));
	ifcx.push_back(Sources->getFormatContext(b));
	iccx.push_back(Sources->getCodecContext(a));
	iccx.push_back(Sources->getCodecContext(b));
	*/
	output_format_context = Sources->getOutFormatContext(0);
	output_codec_context = Sources->getOutCodecContext(0);
	for (int i = 0; i < 3; ++i)
	{
		ifcx.push_back(Sources->getFormatContext(i));
		iccx.push_back(Sources->getCodecContext(i));
	}
	err = init_filter_graph();
	printf("Init err = %d\n", err);

	//process_all();
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/** Initialize one data packet for reading or writing. */
void CFilter::init_packet(AVPacket *packet)
{
	av_init_packet(packet);
	/** Set the packet data and size so that it is recognized as being empty. */
	packet->data = NULL;
	packet->size = 0;
}
//-------------------------------------------------------------------------
/** Decode one audio frame from the input file. */
int CFilter::decode_audio_frame(AVFrame *frame,
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
//-------------------------------------------------------------------------
/** Encode one frame worth of audio to the output file. */
int CFilter::encode_audio_frame(AVFrame *frame,
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

//-------------------------------------------------------------------------
int CFilter::init_input_frame(AVFrame **frame)
{
	if (!(*frame = av_frame_alloc())) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate input frame\n");
		return AVERROR(ENOMEM);
	}
	return 0;
}
//-------------------------------------------------------------------------
int CFilter::process_all(){
	int ret = 0;

	int data_present = 0;
	int finished = 0;

	const int nb_inputs = 3;

	int input_finished[nb_inputs];
	int input_to_read[nb_inputs];
	int total_samples[nb_inputs];
	for (int i = 0; i < nb_inputs; ++i)
	{
		input_finished[i] = 0;
		input_to_read[i] = 1;
		total_samples[i] = 0;
	}

	int total_out_samples = 0;
	int nb_finished = 0;

	while (nb_finished < nb_inputs) {
		int data_present_in_graph = 0;

		for (int i = 0; i < nb_inputs; i++){
			if (input_finished[i] || input_to_read[i] == 0){
				cout << "\n123123";
				continue;
			}
			input_to_read[i] = 0;
			AVFrame *frame = NULL;

			if (init_input_frame(&frame) > 0){
				goto end;
			}

			/** Decode one frame worth of audio samples. */
			if ((ret = decode_audio_frame(frame, ifcx[i], iccx[i], &data_present, &finished))){
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
					goto end;
				}
			}
			else if (data_present) { /** If there is decoded data, convert and store it */
				/* push the audio data from decoded frame into the filtergraph */
				ret = av_buffersrc_write_frame(afcx[i], frame);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error while feeding the audio filtergraph\n");
					goto end;
				}

				av_log(NULL, AV_LOG_INFO, "add %d samples on input %d (%d Hz, time=%f, ttime=%f)\n",
					frame->nb_samples, i, iccx[i]->sample_rate,
					(double)frame->nb_samples / iccx[i]->sample_rate,
					(double)(total_samples[i] += frame->nb_samples) / iccx[i]->sample_rate);

			}

			av_frame_free(&frame);

			data_present_in_graph = data_present || data_present_in_graph;
		}

		if (data_present_in_graph){
			AVFrame *filt_frame = av_frame_alloc();

			/* pull filtered audio from the filtergraph */
			while (1) {
				ret = av_buffersink_get_frame(sink, filt_frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
					for (int i = 0; i < nb_inputs; i++){
						if (av_buffersrc_get_nb_failed_requests(afcx[i]) > 0){
							input_to_read[i] = 1;
							av_log(NULL, AV_LOG_INFO, "Need to read input %d\n", i);
						}
					}

					break;
				}
				if (ret < 0)
					goto end;

				av_log(NULL, AV_LOG_INFO, "remove %d samples from sink (%d Hz, time=%f, ttime=%f)\n",
					filt_frame->nb_samples, output_codec_context->sample_rate,
					(double)filt_frame->nb_samples / output_codec_context->sample_rate,
					(double)(total_out_samples += filt_frame->nb_samples) / output_codec_context->sample_rate);

				//av_log(NULL, AV_LOG_INFO, "Data read from graph\n");
				ret = encode_audio_frame(filt_frame, output_format_context, output_codec_context, &data_present);
				if (ret < 0)
					goto end;

				av_frame_unref(filt_frame);
			}

			av_frame_free(&filt_frame);
		}
		else {
			av_log(NULL, AV_LOG_INFO, "No data in graph\n");
			for (int i = 0; i < nb_inputs; i++){
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
//-------------------------------------------------------------------------
int CFilter::init_filter_graph()
{
	AVFilterGraph *filter_graph;
	AVFilterContext *mix_ctx;
	AVFilter        *mix_filter;
	AVFilterContext *abuffersink_ctx;
	AVFilter        *abuffersink;

	//vector<AVFilterContext *> abuffer_ctx;

	char args[512];

	int err;

	/* Create a new filtergraph, which will contain all the filters. */
	filter_graph = avfilter_graph_alloc();
	if (!filter_graph) {
		av_log(NULL, AV_LOG_ERROR, "Unable to create filter graph.\n");
		return AVERROR(ENOMEM);
	}
	/****** abuffer i ********/
	for (int i = 0; i < 3; ++i)
	{
		AVFilter *abufferTemp;
		AVFilterContext *abufferTemp_ctx;

		/* Create the abuffer filter;
		* it will be used for feeding the data into the graph. */
		abufferTemp = avfilter_get_by_name("abuffer");
		if (!abufferTemp) {
			av_log(NULL, AV_LOG_ERROR, "Could not find the abuffer filter.\n");
			return AVERROR_FILTER_NOT_FOUND;
		}

		if (!iccx[i]->channel_layout)
			iccx[i]->channel_layout = av_get_default_channel_layout(iccx[i]->channels);

		snprintf(args, sizeof(args),
			"sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
			iccx[i]->sample_rate,
			av_get_sample_fmt_name(iccx[i]->sample_fmt), iccx[i]->channel_layout);

		char arg2[5];
		snprintf(arg2, sizeof(arg2), "src%d", i);

		err = avfilter_graph_create_filter(&abufferTemp_ctx, abufferTemp, arg2,
			args, NULL, filter_graph);
		if (err < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
			return err;
		}

		afcx.push_back(abufferTemp_ctx);
	}

	/****** amix ******* */
	/* Create mix filter. */
	mix_filter = avfilter_get_by_name("amix");
	if (!mix_filter) {
		av_log(NULL, AV_LOG_ERROR, "Could not find the mix filter.\n");
		return AVERROR_FILTER_NOT_FOUND;
	}

	snprintf(args, sizeof(args), "inputs=3");

	err = avfilter_graph_create_filter(&mix_ctx, mix_filter, "amix",
		args, NULL, filter_graph);

	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create audio amix filter\n");
		return err;
	}

	/* Finally create the abuffersink filter;
	* it will be used to get the filtered data out of the graph. */
	abuffersink = avfilter_get_by_name("abuffersink");
	if (!abuffersink) {
		av_log(NULL, AV_LOG_ERROR, "Could not find the abuffersink filter.\n");
		return AVERROR_FILTER_NOT_FOUND;
	}

	abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");
	if (!abuffersink_ctx) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate the abuffersink instance.\n");
		return AVERROR(ENOMEM);
	}

	/* Same sample fmts as the output file. */
	/*err = av_opt_set_int_list(abuffersink_ctx, "sample_fmts",
	((int[]){ AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE }),
	AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN);*/
	const enum AVSampleFormat
		Fmts[] = {
		AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE
	};
	err = av_opt_set_int_list(abuffersink_ctx, "sample_fmts",
		Fmts,
		AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN);


	//uint8_t ch_layout[64];
	char ch_layout[64];
	av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, OUTPUT_CHANNELS);
	av_opt_set(abuffersink_ctx, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);

	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could set options to the abuffersink instance.\n");
		return err;
	}

	err = avfilter_init_str(abuffersink_ctx, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not initialize the abuffersink instance.\n");
		return err;
	}

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

	/* Configure the graph. */
	err = avfilter_graph_config(filter_graph, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error while configuring graph : %s\n", get_error_text(err));
		return err;
	}

	char* dump = avfilter_graph_dump(filter_graph, NULL);
	av_log(NULL, AV_LOG_ERROR, "Graph :\n%s\n", dump);

	graph = filter_graph;
	sink = abuffersink_ctx;

	return 0;
}

//-------------------------------------------------------------------------

#endif
