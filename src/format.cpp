#include <string>
#include <sstream>

#include "format.h"

using std::string;

// Helper Function to format time values:
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    string formatted_time;

    // extract hours minutes and seconds from raw seconds value
    int hours = seconds / 3600;
    seconds = seconds % 3600;
    int minutes = seconds / 60;
    seconds = seconds % 60;

    // stream hours, minutes and second values
    // along with leading zeroes and colons
    // into formatted_time string
    std::stringstream time_conversion_stream;\
    if (hours < 10) {
        time_conversion_stream << "0";
    }
    time_conversion_stream << hours << ":";

    if (minutes < 10) {
        time_conversion_stream << "0";
    }
    time_conversion_stream << minutes << ":";

    if (seconds < 10) {
        time_conversion_stream << "0";
    }
    time_conversion_stream << seconds;
    time_conversion_stream >> formatted_time;

    // return seconds converted into correct format.
    return formatted_time;
}