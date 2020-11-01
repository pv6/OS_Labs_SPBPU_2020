#include <iostream>
#include <regex>
#include <ctime>
#include <iomanip>
#include <fstream>

void record_event(int argc, char const* const * argv) {
    std::ofstream config = std::ofstream(argv[1], std::ios_base::app);
    if (!config.is_open())
        throw "Config was not found";

    config << argv[2];
    for (int i = 3; i < argc; i++) config << ' ' << argv[i];
    config << std::endl;
}

void check_time(const std::string date_token, const std::string time_token) {
    std::smatch base_match;
    const std::regex date_regex("\\d{2}.\\d{2}.\\d{4}");
    const std::regex time_regex("\\d{2}:\\d{2}");

    if (!std::regex_match(date_token, date_regex)) {
        throw "Incorrect date format\nValid format dd.mm.yyyy";
    }

    if (!std::regex_match(time_token, time_regex)) {
        throw "Incorrect date format\nValid format hour:minute";
    }

    tm time;
    std::stringstream ss(date_token + time_token);
    ss >> std::get_time(&time, "%d.%m.%Y %H:%M");
    if (ss.fail())
        throw "Incorrect date/time values\n";
}

bool is_flag(const char* token) {
    return token[0] == '-';
}

void check_flag(const char* token) {
    if (token[1] != 'h' && token[1] != 'd' && token[1] != 'w' && is_flag(token))
        throw "Incorrect flag\nValid flags h/d/w";
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cout << "Too few arguments, need 5 at least" << std::endl;
        std::cout << "Config file, date, time, [flag], text" << std::endl;
        return 1;
    }

    try {
        check_time(argv[2], argv[3]);
        check_flag(argv[4]);
        record_event(argc, argv);
    }
    catch (const char* exception) {
        std::cout << "Event recording failed" << std::endl;
        std::cout << "Error:" << exception << std::endl;
        return 1;
    }
    
    return 0;
}
