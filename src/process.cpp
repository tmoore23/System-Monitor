#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::vector;

int Process::Pid() { 
    return pid_; 
}

float Process::CpuUtilization() { 
    return (float)LinuxParser::CpuUtilization(pid_); 
}

string Process::Command() { 
    return LinuxParser::Command(pid_); 
}

string Process::Ram() { 
    ram_ = std::stoi(LinuxParser::Ram(pid_));
    return LinuxParser::Ram(pid_); 
}

string Process::User() {
    return LinuxParser::User(pid_);
}

long int Process::UpTime() { 
    return LinuxParser::UpTime(pid_); 
}

bool Process::operator<(Process const& a) const { 
    return ram_ < a.ram_; 
}