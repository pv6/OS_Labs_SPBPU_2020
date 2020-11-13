#ifndef VIA8_CONN_TYPE_H
#define VIA8_CONN_TYPE_H

enum ConnType {
    CONN_TYPE_NONE = 0,
    CONN_TYPE_MMAP = 1,
    CONN_TYPE_SHM  = 3,
    CONN_TYPE_PIPE = 5
};

#endif /* VIA8_CONN_TYPE_H */
