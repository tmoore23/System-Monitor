#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
using std::map;
using std::stringstream;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  long value;
  string line, key;
  map <string, long> system_map;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        key.pop_back();
        system_map[key] = value;
      }
    }
  }
  
  return (float(system_map["MemTotal"] - system_map["MemFree"]) - float(system_map["Buffers"] - system_map["Cached"])) / 10000000.f; 
}

long LinuxParser::UpTime() { 
  long uptime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

long LinuxParser::Jiffies() { 
  long jiffies = 0;
  for(auto data : LinuxParser::CpuUtilization()) {
    if (data != "cpu") {
      jiffies += stol(data);
    }
  }
  return jiffies;
}

long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_data = LinuxParser::CpuUtilization();
  return stol(cpu_data[CPUStates::kNice_])    + 
         stol(cpu_data[CPUStates::kSystem_])  + 
         stol(cpu_data[CPUStates::kIRQ_])     + 
         stol(cpu_data[CPUStates::kSoftIRQ_]) + 
         stol(cpu_data[CPUStates::kSteal_])   + 
         stol(cpu_data[CPUStates::kGuest_])   + 
         stol(cpu_data[CPUStates::kGuestNice_]);
}

long LinuxParser::IdleJiffies() { 
  vector<string> cpu_data = LinuxParser::CpuUtilization();
  return stol(cpu_data[CPUStates::kIdle_]) + stol(cpu_data[CPUStates::kIOwait_]); 
}

vector<string> LinuxParser::CpuUtilization() {
  string line, user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice;
  vector<string> cpu_utilization;

  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guestnice) {
        if (user == "cpu") {
          cpu_utilization = {user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice};
          return cpu_utilization;
        }
      }
    }
  }

  return cpu_utilization;
}

int LinuxParser::TotalProcesses() { 
  string line, key;
  int total;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> total) {
        if (key == "processes") {
          return total;
        }
      }
    }
  }

  return total;
}

int LinuxParser::RunningProcesses() { 
  string line, key;
  int total;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> total) {
        if (key == "procs_running") {
          return total;
        }
      }
    }
  }

  return total;
}

string LinuxParser::Command(int pid) { 
  string line;
  stringstream directory;
  directory << kProcDirectory << pid << kCmdlineFilename;

  std::ifstream filestream(directory.str());
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }

  return line;
}

string LinuxParser::Ram(int pid) { 
  string line, key;
  int kB;
  stringstream directory;
  directory << kProcDirectory << pid << kStatusFilename;

  std::ifstream filestream(directory.str());
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> kB) {
        key.pop_back();
        if (key == "VmSize") {
          return std::to_string(kB / 1000);
        }
      }
    }
  }

  return std::to_string(kB); 
}

string LinuxParser::Uid(int pid) { 
  string line, key, uid;
  stringstream directory;
  directory << kProcDirectory << pid << kStatusFilename;

  std::ifstream filestream(directory.str());
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> uid) {
        key.pop_back();
        if (key == "Uid") {
          return uid;
        }
      }
    }
  }

  return uid; 
}

string LinuxParser::User(int pid) {
  string line, token, username;
  
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      vector<string> passwd_fields;
      while(std::getline(linestream, token, ':')) {
        passwd_fields.push_back(token);
      }
      // Field 3 represents UID
      if (passwd_fields[2] == LinuxParser::Uid(pid)) {
        // Field 1 represents User
        return passwd_fields[0];
      }
      passwd_fields.clear();
    }
  }

  return username; 
}

long LinuxParser::UpTime(int pid) { 
  string line, key;
  vector<string> status_info;
  stringstream directory;
  directory << kProcDirectory << pid << kStatFilename;

  std::ifstream filestream(directory.str());
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        status_info.push_back(key);
      }
    }
  }

  return stol(status_info[21]) / sysconf(_SC_CLK_TCK);
}

long LinuxParser::CpuUtilization(int pid) { 
  string line, key;
  vector<string> status_info;
  stringstream directory;
  directory << kProcDirectory << pid << kStatFilename;

  std::ifstream filestream(directory.str());
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        status_info.push_back(key);
      }
    }
  }

  /*
   * Calculation is based on the following link:
   * https://stackoverflow.com/a/16736599
   */
  long total_time = stol(status_info[13]) + stol(status_info[14]) + stol(status_info[15]) + stol(status_info[16]);
  long seconds = LinuxParser::UpTime() - (LinuxParser::UpTime(pid) / sysconf(_SC_CLK_TCK));
  return total_time / sysconf(_SC_CLK_TCK) / seconds ;
}