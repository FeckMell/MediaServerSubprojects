#include "stdafx.h"
//struct CAVPacket;

struct CAVPacket : AVPacket
{
	CAVPacket() : AVPacket()
	{
		av_init_packet(this);
		data = nullptr;
		size = 0;
	}
	CAVPacket(size_t sz) : CAVPacket()
	{
		if (sz > 0)
			av_new_packet(this, sz);
	}
	int grow_by(int by)
	{
		return av_grow_packet(this, by);
	}
	void shrink_to(int to)
	{
		av_shrink_packet(this, to);
	}
	~CAVPacket(){ av_free_packet(this); }

	operator bool()const{ return data != nullptr; }
	//void free(){ av_free_packet(this); }
};
typedef shared_ptr<CAVPacket> SHP_CAVPacket;