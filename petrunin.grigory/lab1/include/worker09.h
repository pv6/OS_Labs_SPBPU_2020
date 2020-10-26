#ifndef VIA8_WORKER09_H
#define VIA8_WORKER09_H

#include <string> // string, stoi
#include "worker.h"

class Worker09 final : public Worker {
private:
    std::string m_dir_from;
    std::string m_dir_to;
    int m_interval_s;

    void configure();

public:
    explicit Worker09(std::string const& path_conf);
    ~Worker09() {}
    void work() override;
    void reconfigure() override;

private:
    Worker09(Worker09 const&) = delete;
    Worker09& operator=(Worker09 const&) = delete;
};

#endif /* VIA8_WORKER09_H */