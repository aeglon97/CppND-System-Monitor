#include <iomanip>
#include <string>


#include "format.h"

using std::fixed;
using std::setprecision;

string Format::Pad(string time) {
    //If 1 digit, lead with 0
    time.insert(time.begin(), 2 - time.size(), '0');
    return time;
}
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(const long seconds) { 
    int hoursElapsed = seconds / 3600;
    int minutesElapsed = (seconds % 3600) / 60;
    long secondsElapsed = seconds % 60;

    string hoursDisplay = Format::Pad(to_string(hoursElapsed));
    string minutesDisplay = Format::Pad(to_string(minutesElapsed));
    string secondsDisplay = Format::Pad(to_string(secondsElapsed));
    
    char timeDisplay[100];
    hoursDisplay = sprintf(timeDisplay,
                            "%s:%s:%s",
                            hoursDisplay.c_str(),
                            minutesDisplay.c_str(),
                            secondsDisplay.c_str());
    return timeDisplay;
}

//Round to 2 decimal places
string Format::RoundedFloat(const float num) {
    stringstream stream;
    stream << fixed << setprecision(2) << num;
    return stream.str();
}   