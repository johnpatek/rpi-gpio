#####################################################################
# Copyright © 2021 John R. Patek
# 
# This software is provided 'as-is', without any express or implied 
# warranty. In no event will the authors be held liable for any 
# damages arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any 
# purpose, including commercial applications, and to alter it and 
# redistribute it freely, subject to the following restrictions:
# 
#    1. The origin of this software must not be misrepresented; you 
#       must not claim that you wrote the original software. If you 
#       use this software in a product, an acknowledgment in the 
#       product documentation would be appreciated but is not 
#       required.
#    
#    2. Altered source versions must be plainly marked as such, and 
#       must not be misrepresented as being the original software.
#    
#    3. This notice may not be removed or altered from any source 
#       distribution.
# 
#####################################################################

cmake_minimum_required(VERSION 3.0)

project(rpi-gpio)

set(CMAKE_CXX_STANDARD 14)

set(GPIO_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)    
    set(CMAKE_INSTALL_PREFIX /usr)
endif()

add_subdirectory(src)

install(
    DIRECTORY include/ 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/rpigpio
    PATTERN "include/*.h")

install(
    DIRECTORY demo/ 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/rpigpio/demo)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(THREADS Threads::Threads)

if (WIN32)
    set(PLATFORM "WINDOWS")
    set(PACKAGE "MSI")
elseif (APPLE)
    set(PACKAGE "APPLE")
    set(PLATFORM "APPLE")
    set(GENERATOR "APPLE")
else()
    set(PACKAGE "DEB")
    set(PLATFORM "UBUNTU")
    set(GENERATOR "DEBIAN")
endif()

set(CPACK_PACKAGE_NAME "rpi-gpio")

set(CPACK_GENERATOR ${PACKAGE})


set(CPACK_PACKAGING_INSTALL_PREFIX /usr)

set(CPACK_PACKAGE_CONTACT "John R. Patek <johnpatek2@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION "RPi GPIO")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "John Patek")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES "build")
include (CPack)