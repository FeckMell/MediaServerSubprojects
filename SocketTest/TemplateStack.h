#include "stdafx.h"
typedef std::lock_guard<std::mutex> lockk;
template <typename _T>
class CThreadedCircular : boost::noncopyable
{
public:
	typedef std::unique_lock<std::mutex> ulocker;
	CThreadedCircular(size_t sz, bool bBlockIN = true): buffer_(sz), bBlockIN_(bBlockIN) {bTerminated_ = false;}
	//-----------------------------------------------------------------------
	void push(const _T& val)
	{
		ulocker lk(mutex_);
		if (bBlockIN_)
			condNotFull_.wait(lk, [this]{return !buffer_.full() || bTerminated_; });

		if (!bTerminated_)
		{
			assert(!bBlockIN_ || !buffer_.full());
			buffer_.push_back(val);
			condNotEmpty_.notify_one();
		}
	}
	//-----------------------------------------------------------------------
	bool try_pop(_T& retVal)
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
	_T show(size_t index)
	{
		_T retVal;
		ulocker lk(mutex_);
		condNotEmpty_.wait(lk, [this]{return !buffer_.empty() || bTerminated_; });

		if (!bTerminated_)
		{
			assert(!buffer_.empty());
			retVal = buffer_[index];
			condNotFull_.notify_one();
		}
		return retVal;
	}
	//-----------------------------------------------------------------------
	_T pop()
	{
		_T retVal;
		ulocker lk(mutex_);
		condNotEmpty_.wait(lk, [this]{return !buffer_.empty() || bTerminated_;});

		if (!bTerminated_)
		{
			assert(!buffer_.empty());
			retVal = buffer_.front();
			buffer_.pop_front();
			condNotFull_.notify_one();
		}
		return retVal;
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
	//-----------------------------------------------------------------------
	void resume(){ bTerminated_ = false; }
	bool full()const{ lockk lk(mutex_); return buffer_.full(); }
	bool empty()const{ lockk lk(mutex_); return buffer_.empty(); }
	size_t size()const{ lockk lk(mutex_); return buffer_.size(); }
	bool isTerminated()const{ return bTerminated_ == true; }

private:
	mutable std::mutex			mutex_;
	boost::circular_buffer<_T>	buffer_;
	std::condition_variable		condNotEmpty_;
	std::condition_variable		condNotFull_;
	std::atomic<bool>			bTerminated_;
	const bool	bBlockIN_;

};