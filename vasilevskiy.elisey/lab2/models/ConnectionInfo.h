#ifndef LAB1_CONNECTIONINFO_H
#define LAB1_CONNECTIONINFO_H


class ConnectionInfo {
public:
    ConnectionInfo(int pid) : pid{pid}, attached{pid != 0} {}

    int getPid() const;

    bool isAttached() const;

    void setAttached(bool isAttached);

private:
    int pid;
    bool attached;

};


#endif //LAB1_CONNECTIONINFO_H
