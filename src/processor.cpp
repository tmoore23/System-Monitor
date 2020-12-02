#include "processor.h"

float Processor::Utilization() {
  return (float)(LinuxParser::ActiveJiffies())/ (float)(LinuxParser::Jiffies());
}