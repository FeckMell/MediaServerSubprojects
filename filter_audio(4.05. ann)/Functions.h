#pragma once
#include "stdafx.h"
#include "Structs.h"
#include <boost/type_index.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
string MakeRemotePort(string SDP);
//----------------------------------------------------------------------------
string MakeRemoteIP(string SDP);
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------















/*template <class T>
string GetClass(T t)
{
std::string name = boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name();
return name.substr(name.find_last_of(" ") + 1, name.back());
}*/
/*void logit(string a, int thread, string classname);*/





