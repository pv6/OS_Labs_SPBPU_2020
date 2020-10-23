#include "ConfigReader.h"
#include "Error.h"
#include <fstream>
#include <sstream>

const std::string fieldNames[ConfigReader::numOfFields] = { "FOLDER_1", "FOLDER_2", "UPDATE_TIME", "OLD_DEF_TIME" };

ConfigReader::ConfigReader(const std::string& configFilePath) {
    bool fieldIsParsed[numOfFields] = { false, false, false, false };

    std::ifstream configFile(configFilePath);
    if (!configFile.is_open())
        throw Error::NO_CONFIG_FILE;
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
            // TODO: logging
            throw Error::CONFIG_FAIL;
        }

        // Value of this field is already given
        if (fieldIsParsed[(size_t)field]) {
            // TODO: logging
            throw Error::CONFIG_FAIL;
        }

        _fieldValues[(size_t)field] = line.substr(delimiterPos+1, std::string::npos);
        fieldIsParsed[(size_t)field] = true;
        counter++;
    }

    for (size_t i = 0; i < numOfFields; i++)
        if (!fieldIsParsed[i]) {
            // TODO: logging
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
    try {
        size_t res = std::stoul(_fieldValues[(size_t)Field::OLD_DEF_TIME]);
        return res;
    }
    catch (std::invalid_argument& error) {
        // TODO: logging
        throw Error::CONFIG_FAIL;
    }
}

size_t ConfigReader::getUpdateTime() const {
    try {
        size_t res = std::stoul(_fieldValues[(size_t)Field::UPDATE_TIME]);
        return res;
    }
    catch (std::invalid_argument& error) {
        // TODO: logging
        throw Error::CONFIG_FAIL;
    }
}
