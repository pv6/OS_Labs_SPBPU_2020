#pragma once

#include <cstdlib>

class Conn {
public:
   Conn(int id, bool create) noexcept(false);
   ~Conn();

   bool Read(void* buf, size_t count);
   bool Write(void* buf, size_t count);
};
