cmake_minimum_required(VERSION 3.14)

project(lab1)

set(CMAKE_CXX_STANDARD 17)  # use c++17 (to iterate over files in dir)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} "main.cpp"
	"DaemonCreator.h" "DaemonCreator.cpp"
	"DaemonProcedure.h" "DaemonProcedure.cpp"
	"Helpers.h" "Helpers.cpp"
	"LogLogger.h" "LogLogger.cpp"
	"ProcedureBoss.h" "ProcedureBoss.cpp"
	"Settings.h" "Settings.cpp"
	"SettingsManager.h" "SettingsManager.cpp"
	"SignalHandler.h" "SignalHandler.cpp")
