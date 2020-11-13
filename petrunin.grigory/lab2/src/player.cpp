#include "../include/player.h"
#include "../custom_exception.h"

#include <syslog.h> // syslog, kill
#include <unistd.h> // getpid
#include <fstream>  // ifstream, ofstream
#include <string>   // to_string

std::string const Player::pid_path_host   = "/var/run/lab2/host.pid";
std::string const Player::pid_path_client = "/var/run/lab2/client.pid";

void Player::write_pid(Player::PlayerType type) {
    std::string const& pid_path;
    switch(type) {
    case Player::PlayerType::PLAYER_TYPE_HOST:
        pid_path = Player::pid_path_host;
        break;
    case Player::PlayerType::PLAYER_TYPE_CLIENT:
        pid_path = Player::pid_path_client;
        break;
    default:
        throw CustomException("unknown PlayerType", __FILE__, __LINE__);
    }

    // check whether another instance of program is currently running
    syslog(LOG_DEBUG, "attempting to read pid from file");
    std::ifstream pid_ifstream;
    pid_ifstream.open(pid_path.c_str(), std::ifstream::in);
    if (pid_ifstream.good()) {
        int pid;
        if ((pid_ifstream >> pid) && kill(pid, 0) == 0) {
            pid_ifstream.close();
            throw CustomException(
                ("another instance of program found, PID: " + std::to_string(pid)).c_str(),
                __FILE__,
                __LINE__);
        }
        pid_ifstream.close();
    }

    // save pid to file
    syslog(LOG_DEBUG, "attempting to write pid to file");
    std::ofstream pid_ofstream;
    pid_ofstream.open(pid_path.c_str(), std::ofstream::out);
    if (pid_ofstream.good()) {
        pid_ofstream << (int)getpid();
        pid_ofstream.close();
    }
    else {
        throw CustomException("unable to write pid to file", __FILE__, __LINE__);
    }
    syslog(LOG_DEBUG, "pid has been saved to file");
}