#include "Client.h"
#include "TemperaturePredictor.h"
#include "DateHolder.h"
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <syslog.h>
#include <iostream>


Client::Client(SemWrapper& semHost, SemWrapper& semClient, bool verbose): needStop(false), verbose(verbose), semHost(semHost), semClient(semClient)
{
    //ctor
    bool isHost = false; // it's client
    connection = Conn::createConnection(isHost);
}

Client::~Client()
{
    //dtor
    delete connection;
}


void Client::run() {
    TemperaturePredictor predictor;
    char buf[maxBufSize] = "dd.mm.yyyy";  // init buffer
    while (!needStop) {
	semClient.timedWait();  // wait until client can read
	connection->Read(buf, maxBufSize);
	if (verbose)
	    std::cout << "Client received: " << buf << "\n";
	if (buf[0] == 'e') {  // can exit now, exit msg received
		needStop = true;
		break;
	}
        std::string dateString(buf); // to string
        DateHolder date(dateString); // parse & create a date
        int prediction = predictor.predict(date);  // predict
        std::sprintf(buf, "%i", prediction);  // serialize
        connection->Write(buf, maxBufSize);  // send
	if (verbose)
	    std::cout << "Client wrote: " << buf << "\n";
	semHost.post();  // let host receive the answer
    }
    syslog(LOG_INFO, "Client: exit message received");
}
