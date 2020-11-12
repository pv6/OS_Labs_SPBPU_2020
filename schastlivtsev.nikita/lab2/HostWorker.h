#ifndef HOST_H
#define HOST_H

#include <string>
#include <fstream>
#include "Conn.h"
#include "SemWrapper.h"

class Host
{
    public:
        Host(const std::string& inputFile, const std::string& outputFile, SemWrapper& semHost, SemWrapper& semClient, bool verbose);
	virtual ~Host();

        void run();
    protected:

    private:
        // fields
        static const size_t maxBufSize = 11;
        bool needStop;
	bool verbose = false;
        std::string inputFile;
        std::string outputFile;
        SemWrapper& semHost;
	SemWrapper& semClient;
        Conn* connection;

        // functions
        void ReceiveAnswer(std::ofstream& outfile);
};

#endif // HOST_H
