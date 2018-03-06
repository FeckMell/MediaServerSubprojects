#pragma once
#include "stdafx.h"
#include <condition_variable>

struct CAVFrame
{
	CAVFrame() { frame = NULL; reads = 0; }
	CAVFrame(AVFrame * frame_) { frame = frame_; reads = 0; }
	AVFrame * frame;
	int reads;
};

class CThreadedCircular : boost::noncopyable
{
public:
	typedef std::unique_lock<std::mutex> ulocker;
	typedef std::lock_guard<std::mutex> lock;
	CThreadedCircular(size_t sz, bool bBlockIN = true)
		: buffer_(sz), bBlockIN_(bBlockIN){
		bTerminated_ = false;
	}
	//-----------------------------------------------------------------------
	void push(const CAVFrame& val)
	{
		ulocker lk(mutex_);
		if (bBlockIN_)
		{
			condNotFull_.wait(lk,
				[this]{return !buffer_.full() || bTerminated_; });
		}
		if (!bTerminated_)
		{
			assert(!bBlockIN_ || !buffer_.full());
			buffer_.push_back(val);
			condNotEmpty_.notify_one();
		}
	}
	//-----------------------------------------------------------------------
	bool try_pop(CAVFrame& retVal)
	{
		lock lk(mutex_);
		const bool bOK = !buffer_.empty();
		if (bOK)
		{
			retVal = buffer_.front();
			buffer_.pop_front();
			condNotFull_.notify_one();
		}
		return bOK;
	}

	//-----------------------------------------------------------------------
	CAVFrame pop(int max)
	{
		CAVFrame retVal;
		ulocker lk(mutex_);
		condNotEmpty_.wait(lk, [this]{return (!buffer_.empty() || bTerminated_); });// тут
		if (!bTerminated_)
		{
			assert(!buffer_.empty());

			retVal = buffer_.front();
			buffer_.front().reads++;
			if (retVal.reads == max)
				buffer_.pop_front();			
			condNotFull_.notify_one();
		}
		return retVal;
	}

	//-----------------------------------------------------------------------
	CAVFrame show(int i)
	{
		CAVFrame retVal;
		ulocker lk(mutex_);
		condNotEmpty_.wait(lk, [this]{return (!buffer_.empty() || bTerminated_); });
		if (!bTerminated_)
		{
			assert(!buffer_.empty());
			retVal = buffer_[i];
			condNotFull_.notify_one();
		}
		return retVal;
	}

	//-----------------------------------------------------------------------
	void resume()
	{
		bTerminated_ = false;
	}

	//-----------------------------------------------------------------------
	void terminate()
	{
		bTerminated_ = true;
		lock lk(mutex_);
		buffer_.clear();
		condNotFull_.notify_one();
		condNotEmpty_.notify_one();
	}
	bool full()const{ lock lk(mutex_); return buffer_.full(); }
	bool empty()const{ lock lk(mutex_); return buffer_.empty(); }
	size_t size()const{ lock lk(mutex_); return buffer_.size(); }
	bool isTerminated()const{ return bTerminated_ == true; }
private:
	mutable std::mutex			mutex_;
	boost::circular_buffer<CAVFrame>	buffer_;
	std::condition_variable		condNotEmpty_;
	std::condition_variable		condNotFull_;
	std::atomic<bool>			bcondNotEmpty_ = true;//1
	std::atomic<bool>			bcondNotFull_ = true;//1
	std::atomic<bool>			bTerminated_ = true;
	const bool	bBlockIN_;

};
typedef std::shared_ptr<CThreadedCircular> SHP_CThreadedCircular;