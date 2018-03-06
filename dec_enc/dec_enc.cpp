// dec_enc.cpp : Defines the entry point for the console application.
//


/**
* @file
* libavcodec API use example.
*
* @example decoding_encoding.c
* Note that libavcodec only handles codecs (mpeg, mpeg4, etc...),
* not file formats (avi, vob, mp4, mov, mkv, mxf, flv, mpegts, mpegps, etc...). See library 'libavformat' for the
* format handling
*/
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <math.h>

extern "C"{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
}

#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avcodec.lib")
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avutil.lib")
#pragma comment (lib,"avfilter.lib")

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096


static int check_sample_fmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
{
	const enum AVSampleFormat *p = codec->sample_fmts;

	while (*p != AV_SAMPLE_FMT_NONE) {
		if (*p == sample_fmt)
			return 1;
		p++;
	}
	return 0;
}


static int select_sample_rate(AVCodec *codec)
{
	const int *p;
	int best_samplerate = 0;

	if (!codec->supported_samplerates)
		return 44100;

	p = codec->supported_samplerates;
	while (*p) {
		best_samplerate = FFMAX(*p, best_samplerate);
		p++;
	}
	return best_samplerate;
}


static int select_channel_layout(AVCodec *codec)
{
	const uint64_t *p;
	uint64_t best_ch_layout = 0;
	int best_nb_channels = 0;

	if (!codec->channel_layouts)
		return AV_CH_LAYOUT_STEREO;

	p = codec->channel_layouts;
	while (*p) {
		int nb_channels = av_get_channel_layout_nb_channels(*p);

		if (nb_channels > best_nb_channels) {
			best_ch_layout = *p;
			best_nb_channels = nb_channels;
		}
		p++;
	}
	return best_ch_layout;
}

/*Audio encoding example*/
static void audio_encode_example(const char *filename)
{
	AVCodec *codec;
	AVCodecContext *c = NULL;
	AVFrame *frame;
	AVPacket pkt;
	int i, j, k, ret, got_output;
	int buffer_size;
	FILE *f;
	uint16_t *samples;
	float t, tincr;

	printf("Encode audio file %s\n", filename);

	/* find the MP2 encoder */
	codec = avcodec_find_encoder(/*AV_CODEC_ID_PCM_MULAW*/AV_CODEC_ID_MP2);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}

	c = avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		exit(1);
	}

	/* put sample parameters */
	c->bit_rate = 64000;

	/* check that the encoder supports s16 pcm input */
	c->sample_fmt = AV_SAMPLE_FMT_S16;
	/*if (!check_sample_fmt(codec, c->sample_fmt)) {
		fprintf(stderr, "Encoder does not support sample format %s",
			av_get_sample_fmt_name(c->sample_fmt));
		exit(1);
	}*/

	/* select other audio parameters supported by the encoder */
	c->sample_rate = select_sample_rate(codec);
	c->channel_layout = select_channel_layout(codec);
	c->channels = av_get_channel_layout_nb_channels(c->channel_layout);

	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(2);
	}
	printf("\nqqqq");
	f = fopen(filename, "wb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", filename);
		exit(3);
	}

	/* frame containing input raw audio */
	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate audio frame\n");
		exit(4);
	}
	printf("\nqqqq");
	frame->nb_samples = c->frame_size;
	frame->format = c->sample_fmt;
	frame->channel_layout = c->channel_layout;

	/* the codec gives us the frame size, in samples,
	* we calculate the size of the samples buffer in bytes */
	buffer_size = av_samples_get_buffer_size(NULL, c->channels, c->frame_size,
		c->sample_fmt, 0);
	if (buffer_size < 0) {
		fprintf(stderr, "Could not get sample buffer size\n");
		exit(5);
	}
	samples = (uint16_t *)av_malloc(buffer_size);
	if (!samples) {
		fprintf(stderr, "Could not allocate %d bytes for samples buffer\n",
			buffer_size);
		exit(6);
	}
	/* setup the data pointers in the AVFrame */
	ret = avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt,
		(const uint8_t*)samples, buffer_size, 0);
	if (ret < 0) {
		fprintf(stderr, "Could not setup audio frame\n");
		exit(7);
	}
	printf("\nqqqq");
	/* encode a single tone sound */
	t = 0;
	tincr = 2 * M_PI * 440.0 / c->sample_rate;
	for (i = 0; i < 200; i++) {
		av_init_packet(&pkt);
		pkt.data = NULL; // packet data will be allocated by the encoder
		pkt.size = 0;

		for (j = 0; j < c->frame_size; j++) {
			samples[2 * j] = (int)(sin(t) * 10000);

			for (k = 1; k < c->channels; k++)
				samples[2 * j + k] = samples[2 * j];
			t += tincr;
		}
		/* encode the samples */
		ret = avcodec_encode_audio2(c, &pkt, frame, &got_output);
		if (ret < 0) {
			fprintf(stderr, "Error encoding audio frame\n");
			exit(8);
		}
		if (got_output) {
			fwrite(pkt.data, 1, pkt.size, f);
			av_free_packet(&pkt);
		}
	}

	/* get the delayed frames */
	for (got_output = 1; got_output; i++) {
		ret = avcodec_encode_audio2(c, &pkt, NULL, &got_output);
		if (ret < 0) {
			fprintf(stderr, "Error encoding frame\n");
			exit(9);
		}

		if (got_output) {
			fwrite(pkt.data, 1, pkt.size, f);
			av_free_packet(&pkt);
		}
	}
	fclose(f);

	av_freep(&samples);
	av_frame_free(&frame);
	avcodec_close(c);
	av_free(c);
}

int main(int argc, char **argv)
{
	const char *output_type;

	/* register all the codecs */
	avcodec_register_all();

	/*if (argc < 2) {
		printf("usage: %s output_type\n"
			"API example program to decode/encode a media stream with libavcodec.\n"
			"This program generates a synthetic stream and encodes it to a file\n"
			"named test.h264, test.mp2 or test.mpg depending on output_type.\n"
			"The encoded stream is then decoded and written to a raw data output.\n"
			"output_type must be chosen between 'h264', 'mp2', 'mpg'.\n",
			argv[0]);
		return 1;
	}*/
	//output_type = argv[1];
	output_type = "mp2";

	if (!strcmp(output_type, "mp2")) {
		audio_encode_example("test11.mp2");
		//audio_decode_example("test111.pcm", "test11.mp2");
	}
	else {
		fprintf(stderr, "Invalid output type '%s', choose between 'h264', 'mp2', or 'mpg'\n",
			output_type);
		return 1;
	}

	return 0;
}
