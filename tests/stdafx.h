// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <tchar.h>
#include <winsock2.h> // Wincosk2.h должен быть раньше windows!
#include <windows.h>

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/shared_ptr.hpp>

extern "C"
{
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/avfilter.h>
#include <libavutil/avstring.h>
#include <libavutil/md5.h>
#include <libavutil/mem.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
	//#include <libswresample/swresample.h>
	//#include <libavutil/audio_fifo.h>
	//#include <libavformat/avio.h>
	//#include <libavutil/file.h>
	//#include <libavutil/time.h>
	//#include <libavcodec/avcodec.h>
}


using namespace std;

// TODO: reference additional headers your program requires here
