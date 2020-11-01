#ifndef FILETRANSFER_H_INCLUDED__
#define FILETRANSFER_H_INCLUDED__

#include <string>


class FileTransfer {
    public:
        FileTransfer() = default;
        FileTransfer(std::string dir_from, std::string dir_to);
        void set_values_from_config(std::string dir_from, std::string dir_to);
        bool execute();
        ~FileTransfer() = default;

    protected:
        std::string _dir_from;
        std::string _dir_to;
};

#endif