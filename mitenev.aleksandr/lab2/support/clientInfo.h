//
// Created by aleksandr on 26.11.2020.
//

#ifndef LAB2_CLIENTINFO_H
#define LAB2_CLIENTINFO_H


class ClientInfo {
public:
    int pid;
    bool is_ready;
    int deaths;

    ClientInfo(int pid) : pid(pid), is_ready(pid != 0), deaths(0) {}
};


#endif //LAB2_CLIENTINFO_H
