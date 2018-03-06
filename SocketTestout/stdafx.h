// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>
#ifdef WIN32
#include "targetver.h"
#endif // _WIN32_

#include <stdio.h>
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
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <iomanip>                       
#include <ctime>  

typedef std::lock_guard<std::mutex> lock;
//#include <tchar.h>

// TODO: reference additional headers your program requires here
//#include <boost/asio.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using boost::asio::ip::udp;

//#include <boost/shared_ptr.hpp>
//#include <boost/scoped_ptr.hpp>
#include <boost/format.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdio.h>
#include <iostream>
#include "Mmsystem.h"
#pragma comment (lib,"Winmm.lib")
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using boost::asio::ip::udp;
using namespace boost::asio;
using namespace std;
namespace chrono = std::chrono;
using std::string;
using std::cerr;
using std::cout;
using std::shared_ptr;



// TODO: reference additional headers your program requires here
