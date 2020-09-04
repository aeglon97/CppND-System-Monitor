#include <iomanip>
#include <string>


#include "format.h"

using std::fixed;
using std::setprecision;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(const long seconds) { 
    int hoursElapsed = seconds / 3600;
    int minutesElapsed = (seconds % 3600) / 60;
    int secondsElapsed = seconds % 60;

    char buffer[100];
    hoursElapsed = sprintf(buffer, "%d:%d:%d", hoursElapsed, minutesElapsed, secondsElapsed);
    return buffer;
}

//Round to 2 decimal places
string Format::RoundedFloat(const float num) {
    stringstream stream;
    stream << fixed << setprecision(2) << num;
    return stream.str();
}