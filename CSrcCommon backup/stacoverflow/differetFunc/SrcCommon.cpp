#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "SrcCommon.h"


/************************************************************************
	CSrcCommon	
************************************************************************/
struct SdpOpaque {
	using Vector = std::vector<uint8_t>; Vector data; Vector::iterator pos;
};
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

//-----------------------------------------------------------------------

int CSrcCommon::openold(const string& strFile)
{
	_closeFormat();
	//whatfor?
	//strName_ = str(boost::format("%1%_%2%") % this % strFile);


	m_lastError = 0;
	const char* filename = strFile.c_str();

	BOOST_SCOPE_EXIT_ALL(&)
	{
		if (m_lastError < 0)
			_closeFormat();
	};

	/** Open the input file to read from it. */
	RETURN_AVERROR(
		avformat_open_input(&ctxFormat_, filename, NULL, NULL),
		boost::format("Could not open input file %1%") % filename
		);

	/** Get information on the input file (number of streams etc.). */
	RETURN_AVERROR(
		avformat_find_stream_info(ctxFormat_, NULL),
		"Could not open find stream info"
		);

	//Выбор индекса потока
	for (unsigned i = 0; idxStream_ == -1 && i < ctxFormat_->nb_streams; ++i)
		idxStream_ = ctxFormat_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
		? i : -1;

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
	av_dump_format(ctxFormat_, 0, filename, 0);

	return m_lastError;
}

//-----------------------------------------------------------------------
int CSrcCommon::open(const string& sdp_string)
{
	//_closeFormat();
	//avformat_close_input(&ctxFormat_);
	//sdp_close(&ctxFormat_);
	//printf("\nbind failed1\n");
	m_lastError = 0;
	//av_register_all();
	//avformat_network_init();
	BOOST_SCOPE_EXIT_ALL(&)
	{
		if (m_lastError < 0)
			_closeFormat();
	};
	//printf("\nbind failed1.1\n");
	//AVFormatContext *sdpctx = nullptr;
	const char * sdp_constchar = sdp_string.c_str();
	sdp_open(&ctxFormat_, sdp_constchar, nullptr);

	
	/** Get information on the input file (number of streams etc.). */
	RETURN_AVERROR(
		avformat_find_stream_info(ctxFormat_, nullptr),
		"Could not open find stream info"
		);
	//printf("\nbind failed2\n");
	//Выбор индекса потока
	for (unsigned i = 0; idxStream_ == -1 && i < ctxFormat_->nb_streams; ++i)
		idxStream_ = ctxFormat_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
		? i : -1;
	/*for (size_t i = 0; i < ctxFormat_->nb_streams; ++i) {
		AVStream *st = avformat_new_stream(otherctx, nullptr);
		st->id = i;
		avcodec_copy_context(st->codec, ctxFormat_->streams[i]->codec);
		st->time_base = ctxFormat_->streams[i]->time_base;*/

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
	//printf("\nbind failed3\n");
	//информация о медиа дима вернуть
	av_dump_format(ctxFormat_, 0, "memory.sdp", 0);
	//sdp_close(&ctxFormat_);
	//_closeFormat();
	return m_lastError;
}

//-----------------------------------------------------------------------
void CSrcCommon::_closeFormat()
{
	avformat_close_input(&ctxFormat_);

	//sdp_close(&ctxFormat_);
}
//-----------------------------------------------------------------------
int /*CSrcCommon::*/sdp_read(void *opaque, uint8_t *buf, int size) /*noexcept*/
{
	assert(opaque);
	assert(buf);
	auto octx = static_cast</*CSrcCommon::*/SdpOpaque*>(opaque);

	if (octx->pos == octx->data.end()) {
		return 0;
	}

	auto dist = static_cast<int>(std::distance(octx->pos, octx->data.end()));
	auto count = std::min(size, dist);
	//memcpy(buf,octx->pos,count);
	std::copy(octx->pos, octx->pos + count, buf);
	//basic_string::_Copy_s(octx->pos, octx->pos + count, buf);
	octx->pos += count;

	return count;
}


int CSrcCommon::sdp_open(AVFormatContext **pctx, const char *data, AVDictionary **options) /*noexcept*/
{
	assert(pctx);
	*pctx = avformat_alloc_context();
	assert(*pctx);

	const size_t avioBufferSize = 4096;
	auto avioBuffer = static_cast<uint8_t*>(av_malloc(avioBufferSize));
	auto opaque = new /*CSrcCommon::*/SdpOpaque();
	//printf("\nbind failed1.2\n");
	opaque->data = /*CSrcCommon::*/SdpOpaque::Vector(data, data + strlen(data));
	opaque->pos = opaque->data.begin();
	
	auto pbctx = avio_alloc_context(avioBuffer, avioBufferSize, 0, opaque, &sdp_read, nullptr, nullptr);
	assert(pbctx);
	//printf("\nbind failed1.3\n");
	(*pctx)->pb = pbctx;

	auto infmt = av_find_input_format("sdp");
	//printf("\nbind failed1.4\n");
	return avformat_open_input(pctx, nullptr/*"memory.sdp"*/, /*nullptr*/infmt, nullptr/*options*/);
}

void CSrcCommon::sdp_close(AVFormatContext **fctx) /*noexcept*/
{
	assert(fctx);
	auto ctx = *fctx;

	// Opaque can be non-POD type, free it before and assign to null
	auto opaque = static_cast</*CSrcCommon::*/SdpOpaque*>(ctx->pb->opaque);
	delete opaque;
	ctx->pb->opaque = nullptr;

	avio_close(ctx->pb);
	avformat_close_input(fctx);
}
