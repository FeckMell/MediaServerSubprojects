#include "stdafx.h"
struct SdpOpaque 
{
	using Vector = std::vector<uint8_t>; Vector data; Vector::iterator pos;
};

class CDecoder
{
public:
	CDecoder(const string& sdp_string) { open(sdp_string); }
private:
	int open(const string& sdp_string);
	int sdp_open(AVFormatContext **pctx, const char *data, AVDictionary **options);
	void sdp_close(AVFormatContext **fctx);
	void _closeFormat(){ avformat_close_input(&ctxFormat_); }

	AVCodecContext* CodecCTX()const;
	

	AVFormatContext *ctxFormat_ = nullptr;
	int idxStream_ = -1;
	int m_lastError = 0;
};