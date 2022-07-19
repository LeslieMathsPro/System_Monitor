#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hh, mm, ss;
  stringstream output;
  hh = (seconds / 3600) % 100; 
  mm = (seconds / 60) % 60;
  ss = seconds % 60;
  output << std::setfill('0') << std::setw(2) << hh << ":"
         << std::setfill('0') << std::setw(2) << mm << ":"
         << std::setfill('0') << std::setw(2) << ss;
  return output.str();
}