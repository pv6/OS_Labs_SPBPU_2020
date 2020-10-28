#ifndef DAEMON_H_INCLUDED
#define DAEMON_H_INCLUDED

#include "file_handler.h"

class daemon_parent{
public:
    void create();
    static void signal_handler(int signal_id);
    static void kill_last_daemon();
    virtual void run_process(const char *path, int level = 0) = 0;
    static daemon_parent* getInstance(file_handler* fh);
    virtual ~daemon_parent(){}
protected:
    static file_handler* fh;
    daemon_parent(){}
private:
    daemon_parent(const daemon_parent&);
    daemon_parent& operator=(daemon_parent&);
};

class cur_daemon : public daemon_parent{
public:
    virtual void run_process(const char *path, int level) override;
    static cur_daemon* getInstance(){
       if (!cd_instance)
         cd_instance = new cur_daemon();
       return cd_instance;
    }
    virtual ~cur_daemon(){}
private:
    cur_daemon(){}
    cur_daemon(const cur_daemon&);
    cur_daemon& operator=(cur_daemon&);
    static cur_daemon* cd_instance;
};

#endif // DAEMON_H_INCLUDED
