#pragma once
#include "ISrcFusion.h"
//#define _CRT_SECURE_NO_WARNINGS
/************************************************************************
	CSrcCommon	                                                                     
************************************************************************/
class CSrcCommon : public ISrcFusion, boost::noncopyable
{
public:
	CSrcCommon(){}
	CSrcCommon(const string& sdp_string)
	{
		//printf("CscrCommon1\n");
		open(sdp_string);
	}
	virtual ~CSrcCommon(){
		_closeFormat();
	}
	int openold(const string& strFile);
	int open(const string& sdp_string);
	SHP_CScopedPFrame getNextDecoded(bool& bEOF) override;
	const AVCodecContext *CodecCTX()const override{ return _CodecCTX(); };

	const string& Name() const override { return strName_; }
	/// struct
	
protected:
	AVFormatContext * _CtxFormat() const { return ctxFormat_; }
	AVCodecContext *_CodecCTX()const
	{
		assert(ctxFormat_ != nullptr && idxStream_ >= 0);
		return ctxFormat_->streams[idxStream_]->codec;
	}	
	void _closeFormat();
	int m_lastError = 0;
	
private:
	
	//int sdp_read(void *opaque, uint8_t *buf, int size) /*noexcept*/;
	int sdp_open(AVFormatContext **pctx, const char *data, AVDictionary **options) /*noexcept*/;
	void sdp_close(AVFormatContext **fctx) /*noexcept*/;

	int decode_audio_frame(AVFrame *frame, int *data_present, bool *finished);
	int idxStream_ = -1;
	AVFormatContext *ctxFormat_ = nullptr;
	string strName_;

};
