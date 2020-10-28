#include "ConfigReader.h"
#include "Error.h"
#include <fstream>
#include <syslog.h>
#include <sstream>

const std::string fieldNames[ConfigReader::numOfFields] = { "FOLDER_1", "FOLDER_2", "UPDATE_TIME", "OLD_DEF_TIME" };

ConfigReader::ConfigReader(const std::string& configFilePath) {
    bool fieldIsParsed[numOfFields] = { false, false, false, false };

    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
    	syslog(LOG_ERR, "Can't find configuration file");
        throw Error::NO_CONFIG_FILE;
    }
    std::string line;
    size_t counter = 0;
    while (std::getline(configFile, line) && (counter != numOfFields)) {
        size_t delimiterPos = line.find(delimiter);
        std::string token = line.substr(0, delimiterPos);

        Field field;
        try {
            field = strToField(token);
        }
        catch (Error error) {
            syslog(LOG_ERR, "Failed to parse configuration file [%s], unexpected field %s", configFilePath.c_str(), token.c_str());
            throw Error::CONFIG_FAIL;
        }

        // Value of this field is already given
        if (fieldIsParsed[(size_t)field]) {
            syslog(LOG_ERR, "In configuration file [%s] multiple definition for %s was found", configFilePath.c_str(), token.c_str());
            throw Error::CONFIG_FAIL;
        }

        _fieldValues[(size_t)field] = line.substr(delimiterPos+1, std::string::npos);
        fieldIsParsed[(size_t)field] = true;
        counter++;
    }

    for (size_t i = 0; i < numOfFields; i++)
        if (!fieldIsParsed[i]) {
            syslog(LOG_ERR, "In confiruration file [%s] value of %s wasn't set properly, don't forget that the format is <FIELD>=<FIELD_VALUE>", configFilePath.c_str(), fieldNames[i].c_str());
            throw Error::CONFIG_FAIL;
        }
}

ConfigReader::Field ConfigReader::strToField(const std::string& name) const {
    for (size_t i = 0; i < numOfFields; i++)
        if (name == fieldNames[i])
            return (Field)i;

    throw Error::NO_SUCH_FIELD;
}

std::string ConfigReader::getFolder1Path() const {
    return _fieldValues[(size_t)Field::FOLDER_1];
}

std::string ConfigReader::getFolder2Path() const {
    return _fieldValues[(size_t)Field::FOLDER_2];
}

size_t ConfigReader::getOldDefTime() const {
    size_t index = (size_t)Field::OLD_DEF_TIME;
    try {
        size_t res = std::stoul(_fieldValues[index]);
        return res;
    }
    catch (std::invalid_argument& error) {
        syslog(LOG_ERR, "Field %s value %s is not an unsigned integer", fieldNames[index].c_str(), _fieldValues[index].c_str());
        throw Error::CONFIG_FAIL;
    }
}

size_t ConfigReader::getUpdateTime() const {
    size_t index = (size_t)Field::UPDATE_TIME;
    try {
        size_t res = std::stoul(_fieldValues[(size_t)Field::UPDATE_TIME]);
        return res;
    }
    catch (std::invalid_argument& error) {
        syslog(LOG_ERR, "Field %s value %s is not an unsigned integer", fieldNames[index].c_str(), _fieldValues[index].c_str());
        throw Error::CONFIG_FAIL;
    }
}
