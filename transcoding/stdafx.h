// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define snprintf _snprintf
// std files--------------------------------------------------
#include <stdio.h>
#include <tchar.h>
//#ifdef WIN32
#include "targetver.h"
//#endif // _WIN32_
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <random>
#include <cstdlib>
#include <condition_variable>
// std files end--------------------------------------------------

typedef std::lock_guard<std::mutex> lock;

// TODO: reference additional headers your program requires here

// BOOST FILES--------------------------------------------------
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
// BOOST FILES END --------------------------------------------------

//namespace chrono = std::chrono;
//using std::string;
///using std::cerr;
//using std::cout;
//using std::shared_ptr;
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
// FFMPEG FILES --------------------------------------------------
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/avfilter.h>
#include <libavutil/avstring.h>
	//#include <libswresample/swresample.h>
	//#include <libavutil/audio_fifo.h>
	//#include <libavformat/avio.h>
	//#include <libavutil/file.h>
	//#include <libavutil/time.h>
	//#include <libavcodec/avcodec.h>
}
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avcodec.lib")
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avutil.lib")
#pragma comment (lib,"avfilter.lib")
#define AV_TIME_BASE            1000000
#define AV_TIME_BASE_Q          (AVRational){1, AV_TIME_BASE} 
// FFMPEG FILES END--------------------------------------------------
// TODO: reference additional headers your program requires here
