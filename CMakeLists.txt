cmake_minimum_required(VERSION 2.8.11)
project(dataretreiveusb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(UDEV libudev REQUIRED IMPORTED_TARGET libudev)
pkg_check_modules(CURL libcurl REQUIRED IMPORTED_TARGET libcurl)

include_directories(include/)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${PCLIBUDEV_INCLUDE_DIRS} ${PC_LIBUDEV_INCLUDEDIR})
include_directories(lib/spdlog/include)

# JSON Library
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(lib/json)

set(project_SRC
    src/Settings.cpp
    src/Log.cpp
    src/UsbDevice.cpp
    src/Usb.cpp
    src/DataHandling.cpp

    src/main.cpp
)

set(project_INCLUDE
)

# Tell CMake to create the dataretreiveusb executable
add_executable(dataretreiveusb ${project_SRC} ${project_INCLUDE})

# Tell CMake to link the dataretreiveusb executable
target_link_libraries(dataretreiveusb PkgConfig::UDEV PkgConfig::CURL nlohmann_json::nlohmann_json)

# Install the executable
install(TARGETS dataretreiveusb DESTINATION bin)
