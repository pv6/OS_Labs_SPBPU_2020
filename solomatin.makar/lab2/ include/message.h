#pragma once

namespace message {
    enum class Type {
        CLIENT_HELLO, // client's request to connect to server
        SERVER_HELLO, // server's respond that client connected + client's id
        SERVER_DATE, // server's request for wheather prediction + Date
        CLIENT_WHEATHER, // client's respond + random integer number
    };
    struct Date {
        int Year, Month, Day;
        Date() : Year(0), Month(0), Day(0) {};
    };
}