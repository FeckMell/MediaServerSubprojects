// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define snprintf _snprintf
//#ifdef WIN32
//#include "targetver.h"
//#endif // _WIN32_

#include <stdio.h>
#include <tchar.h>
#ifdef WIN32
#include "targetver.h"
#endif // _WIN32_

//store
#include <string>
#include <vector>
#include <queue>
//#include <map>
//#include <set>

//file
//#include <iostream>
#include <fstream>//
//#include <ostream>

//utils
#include <chrono>
#include <mutex>
#include <thread>
//#include <atomic>
//#include <iomanip>                       
//#include <ctime> 
//#include <exception>


//typedef std::lock_guard<std::mutex> lock;

// TODO: reference additional headers your program requires here
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/exception/all.hpp>
//#include <boost/algorithm/string.hpp>
//#include "boost/date_time/gregorian/gregorian.hpp"
//#include <boost/thread.hpp>



// TODO: reference additional headers your program requires here
