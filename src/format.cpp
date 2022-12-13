#include "format.h"

#include <string>

using std::string;

// Helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;  // In C++ division of positive ints rounds to 0.
  seconds = seconds % 3600;
  int minutes = seconds / 60;
  seconds = seconds % 60;
  string output;
  if (hours < 10) {
    output += "0";
  }
  output += std::to_string(hours) + ":";
  if (minutes < 10) {
    output += "0";
  }
  output += std::to_string(minutes) + ":";
  if (seconds < 10) {
    output += "0";
  }
  output += std::to_string(seconds);

  return output;
}