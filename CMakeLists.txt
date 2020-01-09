cmake_minimum_required(VERSION 2.8.11)
project(dataretreiveusb)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(UDEV libudev REQUIRED IMPORTED_TARGET libudev)

include_directories(include/)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${PCLIBUDEV_INCLUDE_DIRS} ${PC_LIBUDEV_INCLUDEDIR})

set(project_SRC
    src/UsbDevice.cpp
    src/Usb.cpp

    src/main.cpp
)

set(project_INCLUDE
)

# Tell CMake to create the dataretreiveusb executable
add_executable(dataretreiveusb ${project_SRC} ${project_INCLUDE})

# Tell CMake to link the dataretreiveusb executable
target_link_libraries(dataretreiveusb PkgConfig::UDEV)

# Install the executable
install(TARGETS dataretreiveusb DESTINATION bin)
