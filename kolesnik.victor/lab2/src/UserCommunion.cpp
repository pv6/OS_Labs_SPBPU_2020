#include "../include/UserCommunion.h"

#include <iostream>


void UserCommunion::get_clients_number(size_t *n) {
    int _n;

    std::cout << "Please, enter number of clients to create:" << std::endl;
    std::cin >> _n;
    while (_n <= 0) {
        std::cout << "Number of clients can't be 0 or less. Enter again:" << std::endl;
        std::cin >> _n;
    }

    *n = _n;
}
void UserCommunion::get_date(int *day, int *month, int *year) {
    int _day;
    int _month;
    int _year;

    std::cout << "Please, enter the year:" << std::endl;
    std::cin >> _year;
    while (_year <= 0) {
        std::cout << "Year can't be 0 or less. Enter again:" << std::endl;
        std::cin >> _year;
    }

    std::cout << "Please, enter the month:" << std::endl;
    std::cin >> _month;
    while (_month <= 0 || _month > 12) {
        std::cout << "Month is a number from 1 to 12. Enter again:" << std::endl;
        std::cin >> _month;
    }

    std::cout << "Please, enter the day:" << std::endl;
    std::cin >> _day;
    int max_day;
    if (_month == 2) {
        if (_year % 4 == 0) {
            max_day = 29;
        } else {
            max_day = 28;
        }
    } else {
        if (_month == 1 || _month == 3 || _month == 5 || _month == 7 || _month == 8 || _month == 10 || _month == 12) {
            max_day = 31;
        } else {
            max_day = 30;
        }
    }
    while (_day <= 0 || _day > max_day) {
        std::cout << "Day is a number from 1 to" << max_day << ". Enter again:" << std::endl;
        std::cin >> _day;
    }

    *day = _day;
    *month = _month;
    *year = _year;
}
bool UserCommunion::check_exit() {
    char c;
    std::cout << "Would you like to exit? [Y/N]:" << std::endl;
    std::cin >> c;
    while (c != 'y' && c != 'Y' && c != 'n' && c != 'N') {
        std::cout << "Please, enter Y or N:" << std::endl;
        std::cin >> c;
    }
    if (c == 'y' || c == 'Y') {
        return true;
    } else {
        return false;
    }
}
void UserCommunion::put_forecast(int client_id, int temperature) {
    std::cout << "Forecaster #" << client_id << " says the temperaure will be " << temperature << std::endl;
}