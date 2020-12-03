
#include "conn.h"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <map>
#include <unistd.h>



bool conn::Open(size_t id, bool create) {
    this->create = create;
    myId = id;
    if (create){
        fd = (int*) malloc( 2 * sizeof(int));
        int tmp[2];
        //std::cout << "PIPE connection with id = " << id  << std::endl;
        syslog(LOG_NOTICE, "PIPE connection with id = %i", (int)id);
        if (pipe(tmp) == -1){
            perror("pipe");
            //std::cout << "ERROR: pipe failed, error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: pipe failed, error = %s", strerror(errno));
            return false;
        }
        fd[0] = tmp[0];
        fd[1] = tmp[1];
    }
    else{
        //std::cout << "PIPE connection is created with id = " << id << std::endl;
        syslog(LOG_NOTICE, "PIPE connection is creates with id =  %i", (int)id);
    }
    return true;
}

bool conn::Read(void *buf, size_t count) {
    if (read(fd[0], buf, count) != -1) {
        return true;
    }
    //std::cout << "ERROR: read failed with error = " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "ERROR: read failed with error = %s", strerror(errno));
    return false;
}

bool conn::Write(void *buf, size_t count) {
    if (write(fd[1], buf, count) != -1) {
        return true;
    }
    //std::cout << "ERROR: write failed with error = " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "ERROR: write failed with error = %s", strerror(errno));
    return false;
}

bool conn::Close(){

    if (create){
        if ((close(fd[0]) < 0) &&  (close(fd[1]) < 0)){
            free(fd);
            syslog(LOG_ERR, "ERROR: close failed with error = %s", strerror(errno));
            return false;
        }
        free(fd);
    }
    syslog(LOG_NOTICE, "Connection close");
    return true;
}