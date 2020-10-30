#pragma once

#include <string>

namespace CONSTANTS {
    const std::string PID_PATH {"/var/run/daemon.pid"};
    constexpr long MOVE_TIME_BOUND = 10*60;
}
