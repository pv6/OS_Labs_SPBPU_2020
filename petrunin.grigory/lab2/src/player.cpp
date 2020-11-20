#include "../include/player.h"
#include "../include/custom_exception.h"

#include <signal.h> // kill
#include <syslog.h> // syslog
#include <unistd.h> // access, getpid, unlink
#include <fstream>  // ifstream, ofstream
#include <string>   // to_string
#include <errno.h>  // errno, EINTR, ETIMEDOUT
#include <ctime>    // time, timespec, clock_gettime, time_t, CLOCK_REALTIME
#include <cstdlib>  // rand, srand

std::string const Player::semaphore_name_1  = "/via8_lab2_1";
std::string const Player::semaphore_name_2  = "/via8_lab2_2";
std::string const Player::working_dir       = "/var/run/via8_lab2/";
std::string const Player::pid_path_host     = Player::working_dir + "host.pid";
std::string const Player::pid_path_client   = Player::working_dir + "client.pid";

std::string const& Player::get_pid_path(int player_type) {
    srand(time(NULL));
    switch (player_type) {
    case Player::PlayerType::PLAYER_TYPE_HOST:
        return Player::pid_path_host;
    case Player::PlayerType::PLAYER_TYPE_CLIENT:
        return Player::pid_path_client;
    default:
        throw CustomException("unknown PlayerType", __FILE__, __LINE__);
    }
}

Player::Player(int player_type) :
    m_player_type(player_type) {
    switch (player_type) {
    case Player::PlayerType::PLAYER_TYPE_HOST:
    case Player::PlayerType::PLAYER_TYPE_CLIENT:
        break;
    default:
        throw CustomException("unknown PlayerType", __FILE__, __LINE__);
    }
}

Player::~Player() {}

void Player::check_working_dir() const {
    // check access to working directory
    syslog(LOG_DEBUG, "attempting to access working dir");
    if (access(Player::working_dir.c_str(), R_OK | W_OK) != 0) {
        // whatever
        throw CustomException("unable to access working dir, try reinstall app", __FILE__, __LINE__);
    }
}

int Player::read_pid(int player_type) const {
    syslog(LOG_DEBUG, "attempting to read pid from file");
    std::ifstream pid_ifstream;
    pid_ifstream.open(get_pid_path(player_type).c_str(), std::ifstream::in);
    int pid = 0;
    if (pid_ifstream.good()) {
        pid_ifstream >> pid;
        pid_ifstream.close();
    }
    return pid;
}

void Player::write_pid() const {
    check_working_dir();
    
    // check whether another instance of program is currently running
    int pid = read_pid(m_player_type);
    if (pid != 0 && kill(pid, 0) == 0) {
        throw CustomException(
            ("possibly another instance of program runnning, PID: " + std::to_string(pid) + 
            ". if you are sure there's no another instance, try removing pid file"
            "manually: rm " + get_pid_path(m_player_type)).c_str(), __FILE__, __LINE__);
    }

    // save pid to file
    syslog(LOG_DEBUG, "attempting to write pid to file");
    std::ofstream pid_ofstream;
    pid_ofstream.open(get_pid_path(m_player_type).c_str(), std::ofstream::out);
    if (pid_ofstream.good()) {
        pid_ofstream << (int)getpid();
        pid_ofstream.close();
    }
    else {
        throw CustomException("unable to write pid to file", __FILE__, __LINE__);
    }
}

void Player::clear_pid() const {
    syslog(LOG_DEBUG, "attempting to remove pid file");
    if (unlink(get_pid_path(m_player_type).c_str()) != 0) {
        throw CustomException("unlink() error", __FILE__, __LINE__, (int)errno);
    }
}

void Player::post(sem_t* semaphore) const {
    if (sem_post(semaphore) != 0) {
        throw CustomException("sem_post() error", __FILE__, __LINE__, (int)errno);
    }
}

void Player::wait(sem_t* semaphore) const {
    while (sem_wait(semaphore) != 0) {
        if (errno != EINTR) {
        throw CustomException("sem_wait() error", __FILE__, __LINE__, (int)errno);
        }
    }
}

bool Player::timed_wait(sem_t* semaphore, time_t wait_time) const {
    timespec time_spec;
    if (clock_gettime(CLOCK_REALTIME, &time_spec) != 0) {
        throw CustomException("get_clocktime() error", __FILE__, __LINE__, (int)errno);
    }
    time_spec.tv_sec += wait_time;
    int code;
    while ((code = sem_timedwait(semaphore, &time_spec)) == -1) {
        switch (errno) {
        case EINTR:
            // signal interrupt
            if (m_stop) {
                return false;
            }
            continue;
        case ETIMEDOUT:
            // wait time expired
            syslog(LOG_WARNING, "wait time expired");
            return false;
        default:
            throw CustomException("sem_timedwait() error", __FILE__, __LINE__, (int)errno);
        }
    }
    return true;
}

int Player::roll(int min, int max) const {
    return min + (std::rand() % (max - min + 1));
}
