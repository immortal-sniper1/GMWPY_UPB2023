# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# (Not part of the boilerplate)
# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


# set(EXTRA_COMPONENT_DIRS C:/Users/beia/esp-idf-lib/components)   #for beia laptorp
set(EXTRA_COMPONENT_DIRS E:/IDF-espressif/COMPONENTS/esp-idf-lib/components)   #for My laptorp
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common/include/protocol_examples_common.h)

project(mqtt_tcp)
