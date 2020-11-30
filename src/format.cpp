#include <string>
#include <sstream>

#include "format.h"

using std::string;
using std::stringstream;

string Format::ElapsedTime(long totalseconds) { 
   int seconds = totalseconds % 60;
   int minutes = (totalseconds / 60) % 60;
   int hours = (totalseconds / 3600) % 24;

    stringstream result;
    if (hours < 10) {
        result << "0" << hours << ":" << minutes << ":" << seconds;
    } else {
        result << hours << ":" << minutes << ":" << seconds;
    }

    return result.str(); 
}