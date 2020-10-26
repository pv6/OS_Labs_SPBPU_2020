#include "SettingsManager.h"
#include "Helpers.h"
#include <iostream>

SettingsManager::SettingsManager()
{
    //ctor
}

SettingsManager::~SettingsManager()
{
    //dtor
    if (configName) {
        delete [] configName;
        configName = nullptr;
    }
}


void SettingsManager::linkEveryone(ProcedureBoss* procBoss, LogLogger* runnable) {
    procedureBoss = procBoss;
    logLogger = runnable;
}


void SettingsManager::updateSettings() {
    if (!configName)
        throw std::runtime_error("Tried to update settings without config file name");
    settings.parseConfig(configName);
    assignSettings();
}


void SettingsManager::finishSignal() {
    procedureBoss->finishWork();
}


void SettingsManager::setConfig(char const * const filename) {
    if (!filename)
        throw std::runtime_error("Null pointer passed as config name into SettingsManager::setConfig");
    Helpers::copyCStr(&configName, filename);
}


void SettingsManager::assignSettings() {
    if (!procedureBoss || !logLogger)
        throw std::runtime_error("Can't assign settings, not all objects initialized");
    procedureBoss->setWaitSec(settings.getWaitSec());
    logLogger->setTotalLogName(settings.getTotalLogName());
    logLogger->setDirs(settings.getFolderSrc(), settings.getFolderDest());
}
