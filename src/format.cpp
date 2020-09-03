#include <iomanip>
#include <string>


#include "format.h"

using std::fixed;
using std::setprecision;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int hours_elapsed, minutes_elapsed, seconds_elapsed;
    hours_elapsed = seconds / 3600;
    minutes_elapsed = (seconds % 3600) / 60;
    seconds_elapsed = seconds % 60;

    char buffer[100];
    hours_elapsed = sprintf(buffer, "%d:%d:%d", hours_elapsed, minutes_elapsed, seconds_elapsed);
    return buffer;
}

//Round to 2 decimal places
string Format::RoundedFloat(float num) {
    stringstream stream;
    stream << fixed << setprecision(2) << num;
    return stream.str();
}