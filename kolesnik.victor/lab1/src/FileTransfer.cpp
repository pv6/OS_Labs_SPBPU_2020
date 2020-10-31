#include "../include/FileTransfer.h"


FileTransfer::FileTransfer(std::string dir_from, std::string dir_to)
    : _dir_from(dir_from), _dir_to(dir_to)
{}
void FileTransfer::set_values_from_config(std::string dir_from, std::string dir_to) {
    _dir_from = dir_from;
    _dir_to = dir_to;
}
bool FileTransfer::execute() {
    if (!(_dir_from.empty() || _dir_to.empty())) {
        std::string space = " ";
        std::string star = "/*";
        std::string png_star = "/*.png";
        std::string mkdir = "mkdir";
        std::string rm = "rm";
        std::string rf_flag = "-rf";
        std::string r_flag = "-r";
        std::string cp = "cp";
        std::string mv = "mv";

        std::string img_folder = "/IMG";
        std::string other_folder = "/OTHERS";


        std::string command = rm + space + rf_flag + space + _dir_to + star;
        if (system(command.c_str()) != 0) {
            return false;
        }

        command = mkdir + space + _dir_to + img_folder;
        if (system(command.c_str()) != 0) {
            return false;
        }

        command = mkdir + space + _dir_to + other_folder;
        if (system(command.c_str()) != 0) {
            return false;
        }

        command = cp + space + r_flag + space + _dir_from + star + space + _dir_to + other_folder;
        if (system(command.c_str()) != 0) {
            return false;
        }

        command = mv + space + _dir_to + other_folder + png_star + space + _dir_to + img_folder;
        if (system(command.c_str()) != 0) {
            return false;
        }

        return true;
    } else {
        return false;
    }
}