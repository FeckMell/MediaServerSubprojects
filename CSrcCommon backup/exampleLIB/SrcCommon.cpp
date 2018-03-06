#include "stdafx.h"
#include "SrcCommon.h"


/************************************************************************
	CSrcCommon	
************************************************************************/
int CSrcCommon::decode_audio_frame(AVFrame *frame,
	int *gotted, bool *finished)
{
	/** Packet used for temporary storage. */
	CAVPacket input_packet;
	int error = 0;

	/** Read one audio frame from the input file into a temporary packet. */
	if ((error = av_read_frame(ctxFormat_, &input_packet)) < 0) {
		/** If we are the the end of the file, flush the decoder below. */
		*finished = true;

		/*
		if (error == AVERROR_EOF)
		*finished = 1;
		else {
		av_log(NULL, AV_LOG_ERROR, "Could not read frame (error '%s')\n",
		get_error_pchar(error));
		return error;
		}
		*/
	}

	/**
	* Decode the audio frame stored in the temporary packet.
	* The input audio stream decoder is used to do this.
	* If we are at the end of the file, pass an empty packet to the decoder
	* to flush it.
	*/
	RETURN_AVERROR(avcodec_decode_audio4(_CodecCTX(), frame, gotted, &input_packet),
		"Could not decode frame"
		);

	/**
	* If the decoder has not been flushed completely, we are not finished,
	* so that this function has to be called again.
	*/
	if (*finished && *gotted)
		*finished = false;


	return 0;
}

//-----------------------------------------------------------------------
SHP_CScopedPFrame CSrcCommon::getNextDecoded(bool& bEOF)
{
	SHP_CScopedPFrame shResult;

	/** Packet used for temporary storage. */
	CAVPacket packIN;
	int error = av_read_frame(ctxFormat_, &packIN);

	/** Read one audio frame from the input file into a temporary packet. */
	if (error < 0) {
		m_lastError = error;
		/** If we are the the end of the file, flush the decoder below. */
		bEOF = true;
		return shResult;
		/*
		if (error == AVERROR_EOF)
		*finished = 1;
		else {
		av_log(NULL, AV_LOG_ERROR, "Could not read frame (error '%s')\n",
		get_error_pchar(error));
		return error;
		}
		*/
	}

	/**
	* Decode the audio frame stored in the temporary packet.
	* The input audio stream decoder is used to do this.
	* If we are at the end of the file, pass an empty packet to the decoder
	* to flush it.
	*/
	int gotted = 0;
	shResult = std::make_shared<CScopedPFrame>();

	m_lastError = avcodec_decode_audio4(_CodecCTX(), *shResult, &gotted, &packIN);
	if (m_lastError < 0)
	{
		cerr << "Could not decode frame";
		return SHP_CScopedPFrame();
	}


	/**
	* If the decoder has not been flushed completely, we are not finished,
	* so that this function has to be called again.
	*/
	if (bEOF && gotted)
		bEOF = false;

	return shResult;
}


int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
	struct CSrcCommon::buffer_data *bd = (struct CSrcCommon::buffer_data *)opaque;
	buf_size = FFMIN((signed)buf_size, (signed)bd->size);
	//printf("ptr:%p size:%zu\n", bd->ptr, bd->size);
	/* copy internal buffer data to buf */
	memcpy(buf, bd->ptr, buf_size);
	bd->ptr += buf_size;
	bd->size -= buf_size;
	return buf_size;
}
//-----------------------------------------------------------------------
int CSrcCommon::open(const string& sdp_string)
{
	_closeFormat();
	//whatfor?
	//strName_ = str(boost::format("%1%_%2%") % this % strFile);


	m_lastError = 0;
	//const char* filename = strFile.c_str();

	BOOST_SCOPE_EXIT_ALL(&)
	{
		if (m_lastError < 0)
			_closeFormat();
	};

	ctxFormat_ = nullptr;
	AVIOContext *avio_ctx = nullptr;
	uint8_t /**buffer = NULL,*/ *avio_ctx_buffer = nullptr;
	size_t /*buffer_size,*/ avio_ctx_buffer_size = 4096;
	//char *input_filename = NULL;
	int ret = 0;
	struct CSrcCommon::buffer_data bd = { 0 };

	m_lastError = 0; //protected параметр класса
	std::vector<uint8_t> SDPvector(sdp_string.begin(), sdp_string.end()); // преобразуем string в uint8_t с которым работает ffmpeg

	/* fill opaque structure used by the AVIOContext read callback */
	bd.ptr = &SDPvector[0];//buffer
	bd.size = 4096;//buffer_size;

	BOOST_SCOPE_EXIT_ALL(&)
	{
		if (m_lastError < 0)
			_closeFormat();
	};
	if (!(ctxFormat_ = avformat_alloc_context())) {
		boost::format("problem with avformat_alloc_context111");
		ret = AVERROR(ENOMEM);
	}
	avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size); 
	if (!avio_ctx_buffer) {
		boost::format("problem with av_malloc");
		ret = AVERROR(ENOMEM);
	}
	//avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
	//  0, &bd, &read_packet, NULL, NULL); -в примере библиотеки стоит так, но в этот раз компилятор ругается
	//на static int read_packet
	avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
		0, &bd, &read_packet, nullptr, nullptr);
	if (!avio_ctx) {
		boost::format("problem with avio_alloc_context222");
		ret = AVERROR(ENOMEM);
	}
	ctxFormat_->pb = avio_ctx;

	printf("OPEN 6\n");// падает после этого
	/** Open the input file to read from it. */
	ret = avformat_open_input(&ctxFormat_, nullptr, nullptr, nullptr);
	if (ret < 0) { fprintf(stderr, "Could not open input\n"); }

	printf("OPEN 7\n");
	/** Get information on the input file (number of streams etc.). */
	ret = avformat_find_stream_info(ctxFormat_, nullptr);
	if (ret < 0) { fprintf(stderr, "Could not find stream information\n"); }
	printf("OPEN 8\n");
	//Выбор индекса потока
	for (unsigned i = 0; idxStream_ == -1 && i < ctxFormat_->nb_streams; ++i){
		//printf("\ni= %d\n", i);
		idxStream_ = ctxFormat_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
			? i : -1;
	}
	printf("OPEN 9\n");
	/** Find a decoder for the audio stream. */
	AVCodec *input_codec = avcodec_find_decoder(_CodecCTX()->codec_id);
	if (!input_codec)
		RETURN_AVERROR(AVERROR_EXIT, "Could not find input codec");

	/** Open the decoder for the audio stream to use it later. */
	RETURN_AVERROR(
		avcodec_open2(_CodecCTX(), input_codec, nullptr),
		"Could not open input codec"
		);

	if (!_CodecCTX()->channel_layout)
		_CodecCTX()->channel_layout = av_get_default_channel_layout(_CodecCTX()->channels);

	//информация о медиа
	//av_dump_format(ctxFormat_, 0, "outSDP.sdp", 0);
	printf("OPEN 10\n");
	return m_lastError;
}

//-----------------------------------------------------------------------
void CSrcCommon::_closeFormat()
{
	avformat_close_input(&ctxFormat_);
}
