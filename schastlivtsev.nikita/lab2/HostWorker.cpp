#include "HostWorker.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <syslog.h>
#include <iostream>
#include "DateHolder.h"

Host::Host(const std::string& inputFile, const std::string& outputFile, SemWrapper& semHost, SemWrapper& semClient, bool verbose) : needStop(false), verbose(verbose), inputFile(inputFile), outputFile(outputFile), semHost(semHost), semClient(semClient) 
{
    //ctor
    bool isHost = true;  // it's host
    connection = Conn::createConnection(isHost);
}

Host::~Host()
{
    //dtor
    delete connection;
}

void Host::run() {
    std::ifstream infile(inputFile, std::ios::in);
    std::ofstream outfile(outputFile, std::ios::out);
    try {  // have to close files before passing exception to main try-catch
    if (!infile.good())
        throw std::runtime_error("Host: can't open input file");
    if (!outfile.good())
        throw std::runtime_error("Host: can't open output file");
    std::string currentLine;
    while (!needStop && !infile.eof()) {
        // main host procedure
        std::getline(infile, currentLine);
	if (currentLine.length() != 10) {  // not a date
		needStop = true;
		break;
	}
	semHost.timedWait();  // wait until host can write
	if (verbose)
	    std::cout << "Host writing: " << currentLine << "\n";
	connection->Write(currentLine.c_str(), currentLine.length());
        semClient.post();  // let client read & write
	semHost.timedWait(); // wait until host can read
        ReceiveAnswer(outfile);
	semHost.post();
    }
    syslog(LOG_INFO, "Host: the last line processed");
    semHost.timedWait();  // wait until host can write
    connection->Write("e", 1);  // send exit message to the client
    if (verbose)
	std::cout << "Host writing exit message\n";
    semClient.post();  // let client receive exit message
    } catch(std::runtime_error& err) {
        infile.close();
        outfile.close();
        throw err;
    }
    infile.close();
    outfile.close();
}


void Host::ReceiveAnswer(std::ofstream& outfile) {
    char buf[maxBufSize] = "dd.mm.yyyy";
    connection->Read(buf, maxBufSize);
    if (verbose)
        std::cout << "Host received: " << buf << "\n";
    outfile.write(buf, std::strlen(buf)); // buf is the string representation of the answer already
    outfile.write("\n", 1);
}
