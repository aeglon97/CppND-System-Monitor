#ifndef FORMAT_H
#define FORMAT_H

#include <string>

using namespace std;

namespace Format {
    string Pad(string time);
    string ElapsedTime(long times);
    string RoundedFloat(float num);
};

#endif