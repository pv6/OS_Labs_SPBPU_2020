#ifndef LAB2_CONNECT_H
#define LAB2_CONNECT_H


#include <stdlib.h>

class connection
{
public:
    connection();
    ~connection();

    bool open_connect(size_t id, bool create);
    bool close_connect();

    bool recv_connect(void *buf, size_t count);
    bool send_connect(void *buf, size_t count);

private:
    bool need_to_rm, is_open;
    int *desc;
};

#endif //LAB2_CONNECT_H
