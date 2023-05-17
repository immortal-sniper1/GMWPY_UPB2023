# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#set(EXTRA_COMPONENT_DIRS C:/Users/beia/esp-idf-lib/components)   #for beia laptorp
set(EXTRA_COMPONENT_DIRS E:/IDF-espressif/COMPONENTS/esp-idf-lib/components)   #for My laptorp

project(sd_card)
