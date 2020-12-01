#include "processor.h"
#include <string>

using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  string user, line;
  float nice, system, idle, iowait, irq, softirq;
  float total = 0.0;

  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq) {
        if (user == "cpu") {
          return (nice + system + idle + iowait + irq + softirq) / LinuxParser::UpTime();
        }
      }
    }
  }

  return total;
}