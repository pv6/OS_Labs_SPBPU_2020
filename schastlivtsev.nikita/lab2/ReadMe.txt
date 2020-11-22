Lab 2, variant 13

Script to compile the whole project and get 3 executables: complieProject.sh
Executables: host_mq (message queue), host_fifo (FIFO IPC file), host_sock (socket)

Run executables with 2 or 3 arguments:
arg1: the input file name. The input file has to be a text file with dates, dates format: "dd.mm.yyyy". Each date has to be in a single line with no space symbols except the line ending, no more than one date in a single line should be represented.

arg2: the output file name. Host will write predictions it received from the client for each date written in the input file.

arg3 (optional): verbose mode, 1 to make Host & Client write all messages they send or receive to stdout. If the value is not 1 or even missed, nothing will be written to stdout.

All status info (including errors) will be written to the syslog. You can run seeJournal.sh script to open syslog with "less" viewer.

testExample contain the example of the input file.

Example, how to launch the program:

# compile at once
./compileProject.sh

# run socket implementation, for example
./host_sock testExample/in.txt testExample/out.txt
# look results
cat testExample/out.txt

# run message queue implementation in verbose mode
./host_mq testExample/in.txt testExample/out.txt 1
# look results
cat testExample/out.txt

