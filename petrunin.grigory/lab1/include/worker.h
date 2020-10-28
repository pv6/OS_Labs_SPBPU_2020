#ifndef VIA8_WORKER_H
#define VIA8_WORKER_H

#include <string> // string

class Worker {
protected:
    std::string m_path_conf;
    bool m_stop = false;

public:
    explicit Worker(std::string const& path_conf);
    virtual ~Worker() {}
    virtual void work() = 0;
    virtual void reconfigure() = 0;
    void stop() { m_stop = true; }
    
private:
    Worker(Worker const&) = delete;
    Worker& operator=(Worker const&) = delete;
};

#endif /* VIA8_WORKER_H */