cmake_minimum_required(VERSION 3.16)

set(SDKCONFIG "${CMAKE_SOURCE_DIR}/../sdkconfig")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(moonbit_esp32_project)
