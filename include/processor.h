#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // Declare any necessary private members
 private:
 static const int minUptimeInterval = 2; // seconds
 int lastUptime = 0;
 float lastActiveJiffies = 0;
 float lastTotalJiffies = 1; // to avoid a divide by zero error
 float lastUtilization = 0;
};

#endif