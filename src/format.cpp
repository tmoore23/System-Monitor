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
        result << "0" << hours;
    } else {
        result << hours;
    }
        
    if (minutes < 10) {
        result << ":" << "0" << minutes << ":" << seconds;
    } else {
        result << ":" << minutes << ":" << seconds;
    }

    return result.str(); 
}