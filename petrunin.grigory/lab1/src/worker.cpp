#include "../include/worker.h"
#include "../include/exceptions.h"

#include <stdlib.h> // realpath
#include <limits.h> // PATH_MAX

Worker::Worker(std::string const& path_conf) {
    // save absolute path
	char* buff = new char[PATH_MAX];
	if (realpath(path_conf.c_str(), buff) == NULL) {
        delete[] buff;
        throw InvalidPathException();
    }
	m_path_conf = std::string(buff);
	delete[] buff;
}
