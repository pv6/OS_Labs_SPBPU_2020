#pragma once

#include <cstdlib>

class Conn {
public:
    Conn(int id, bool create) noexcept(false);
    ~Conn();

    bool Read(void* buf, size_t count) noexcept;
    bool Write(void* buf, size_t count) noexcept;

    struct Msg {
        enum class TYPE
        {
            TO_WOLF = 0,
            TO_GOAT
        };
        TYPE type;
        int data;
    };

private:
    bool _owner;
    int _hostPid;
    int _desc;
    void* _pData;
};
