#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    long upTime = LinuxParser::UpTime();
    float utilization = lastUtilization; // Default to the most recent calculation
    // Update utilization if at least minUptimeInterval seconds has passed
    if (upTime - lastUptime > minUptimeInterval) {
        long activeJiffies = LinuxParser::ActiveJiffies();
        long totalJiffies = LinuxParser::Jiffies();
        utilization = (activeJiffies - lastActiveJiffies)/(totalJiffies - lastTotalJiffies);
        lastActiveJiffies = activeJiffies;
        lastTotalJiffies = totalJiffies;
        lastUptime = upTime;
        lastUtilization = utilization;
    }
    return utilization;
}