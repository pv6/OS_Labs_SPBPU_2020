#ifndef HOST_CONN_INF_H_INCLUDED
#define HOST_CONN_INF_H_INCLUDED

class HostConnInfo {
public:
    HostConnInfo(int pid) : pid{pid}, attached{pid != 0} {}

    int getPid(){return this->pid;}

    bool isAttached(){return this->attached;}

    void setAttached(bool isAttached){this->attached = isAttached;}

private:
    int pid;
    bool attached;

};

#endif // HOST_CONN_INF_H_INCLUDED
