#include <string>

#include "format.h"

using namespace std;
using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours_elapsed, minutes_elapsed, seconds_elapsed;
    hours_elapsed = seconds / 3600;
    minutes_elapsed = (seconds % 3600) / 60;
    seconds_elapsed = seconds % 60;

    char buffer[100];
    hours_elapsed = sprintf(buffer, "%d:%d:%d", hours_elapsed, minutes_elapsed, seconds_elapsed);
    return buffer;
}