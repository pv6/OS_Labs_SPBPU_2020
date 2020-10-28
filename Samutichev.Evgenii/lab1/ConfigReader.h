#ifndef CONFIGREADER_H_INCLUDED
#define CONFIGREADER_H_INCLUDED

#include <string>

class ConfigReader {
public:
    explicit ConfigReader(const std::string& configFilePath);

    size_t getUpdateTime() const;
    size_t getOldDefTime() const;
    std::string getFolder1Path() const;
    std::string getFolder2Path() const;

    static const size_t numOfFields = 4;
    const std::string delimiter = "=";

private:
    enum class Field {
        FOLDER_1 = 0, FOLDER_2 = 1, UPDATE_TIME = 2, OLD_DEF_TIME = 3
    };

    std::string _fieldValues[numOfFields];

    Field strToField(const std::string& name) const;
};

#endif // CONFIGREADER_H_INCLUDED
