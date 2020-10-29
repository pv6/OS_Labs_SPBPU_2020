#include "Manager.h"

#define CONFIG_NAME "config.txt"

Manager& Manager::getInstance()
{
    static Manager inst;
    return inst;
}

Manager::Manager()
{
    commandList_.emplace_back("Add event to config file");
    commandList_.emplace_back("Check and print all events");
    commandList_.emplace_back("Run daemon");
    commandList_.emplace_back("Exit");

    testEventList_.emplace_back("Incorrect date (dd.mm.yyyy)");
    testEventList_.emplace_back("Incorrect time (hh:mm:ss)");
    testEventList_.emplace_back("Invalid date or time (hours > 23, month > 12 and etc.)");
}

void Manager::printCommandList()
{
    int indexCommand = 0;

    std::cout << std::endl;
    for (std::string &s : commandList_)
    {
        std::cout << ++indexCommand << " - " << s << std::endl;
    }
    std::cout << std::endl;
}

void Manager::printConfigFile()
{
    std::ifstream fIn(CONFIG_NAME);
    std::string tmp;

    std::list<std::string> listCorrectEvent;
    bool needRewrite = false;

    std::cout << std::endl;
    if (fIn.is_open())
    {
        std::stringstream strIn;
        std::string eventDate;
        std::string eventTime;
        std::string eventFlag;


        int countIncorrectEvent = 0;

        while (!fIn.eof())
        {
            getline(fIn, tmp);
            if (!tmp.empty())
            {
                strIn << tmp;
                strIn >> eventDate >> eventTime >> eventFlag;

                if (checkEventDateAndTime(eventDate, eventTime, false) &&
                    (eventFlag.find("-m") || eventFlag.find("-h") ||
                    eventFlag.find("-d") || eventFlag.find("-w")))
                {
                    std::cout << tmp << " - Correct event" << std::endl;
                    listCorrectEvent.emplace_back(tmp);
                }
                else
                {
                    std::cout << tmp << " - Incorrect event" << std::endl;
                    countIncorrectEvent++;
                }
                strIn.str(std::string());
                strIn.clear();
            }
        }
        if (countIncorrectEvent)
        {
            char ans;

            std::cout << std::endl;
            std::cout << countIncorrectEvent << " invalid events. Someone changed the file" << std::endl;
            std::cout << "Do you want to delete incorrect events? (Y/N) ";
            std::cin >> ans;
            if (ans == 'Y' || ans == 'y')
            {
                needRewrite = true;
            }
            getchar();

        }
    }
    std::cout << std::endl;

    fIn.close();

    if (needRewrite)
    {
        std::ofstream fOut(CONFIG_NAME);
        if (fOut.is_open())
        {
            for (std::string &str : listCorrectEvent)
            {
                fOut << str << std::endl;
            }
        }
        fOut.close();
    }
}

void Manager::writeToConfigFile(std::string &eventDate, std::string &eventTime, std::string &eventFlag, std::string &eventText)
{
    std::ofstream fOut(CONFIG_NAME, std::ios::app);
    if (fOut.is_open())
    {
        fOut << eventDate << " " << eventTime << " " << eventFlag << " " << eventText << std::endl;
    }
    fOut.close();
}

bool Manager::checkEventDateAndTime(std::string &eventDate, std::string &eventTime, bool needWriteToConsole)
{
    size_t n = testEventList_.size();
    bool* test = new bool[n] {false};
    tm t{};

    //Test 1
    if (sscanf(eventDate.c_str(),"%d.%d.%d", &t.tm_mday, &t.tm_mon, &t.tm_year) != 3)
    {
        test[0] = true;
    }

    //Test 2
    if (sscanf(eventTime.c_str(),"%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec) != 3)
    {
        test[1] = true;
    }

    //Test 3
    tm tCheck = t;
    mktime(&tCheck);
    if (t.tm_year != tCheck.tm_year || t.tm_mon != tCheck.tm_mon || t.tm_mday != tCheck.tm_mday ||
        t.tm_hour != tCheck.tm_hour || t.tm_min != tCheck.tm_min || t.tm_sec != tCheck.tm_sec)
    {
        test[2] = true;
    }

    int index = 0;
    bool isIncorrectEvent = false;

    std::cout << std::endl;
    for (std::string &errorName: testEventList_)
    {
        if (test[index])
        {
            if (needWriteToConsole)
            {
                std::cout << "Test " << index << " - " << errorName << std::endl;
            }
            isIncorrectEvent = true;
        }
        index++;
    }

    delete[] test;
    return !isIncorrectEvent;
}

void Manager::addEvent()
{
    std::string eventDate;
    std::string eventTime;
    std::string eventFlag;
    std::string eventText;

    std::cout << "Enter the event in the format:" << std::endl;
    std::cout << "dd.mm.yyyy hh:mm:ss [flag] text" << std::endl;
    std::cout << R"(flag - optional argument from "-h", "-d" or "-w")" << std::endl << std::endl;

    std::stringstream strIn;
    std::string tmpStr;
    getline(std::cin,tmpStr);

    strIn << tmpStr;
    strIn >> eventDate >> eventTime >> eventFlag;

    if (checkEventDateAndTime(eventDate, eventTime, true))
    {
        if (eventFlag.find("-h") != std::string::npos || eventFlag.find("-d") != std::string::npos ||
            eventFlag.find("-w") != std::string::npos)
        {
            getline(strIn, eventText);
        }
        else
        {
            eventText = eventFlag;
            eventFlag = "-m";
        }
        writeToConfigFile(eventDate, eventTime, eventFlag, eventText);
        std::cout << "Correct event. Added to config file." << std::endl << std::endl;
    }
    else
    {
        std::cout << "Incorrect event!" << std::endl << std::endl;
    }

}

void Manager::runDaemon()
{
    system("./daemon config.txt");
}

void Manager::run()
{
    int command;
    bool flag = true;

    while (flag)
    {
        printCommandList();

        std::cin >> command;
        switch (command - 1)
        {
            case COMMAND_ADD_EVENT:
                getchar();
                addEvent();
                break;
            case COMMAND_PRINT_CONFIG_AND_CHECK:
                printConfigFile();
                break;
            case COMMAND_RUN_DAEMON:
                runDaemon();
                break;
            case COMMAND_EXIT:
                flag = false;
                break;
            default:
                std::cout << "Invalid command!\n Try again." << std::endl;
                break;
        }
    }
}