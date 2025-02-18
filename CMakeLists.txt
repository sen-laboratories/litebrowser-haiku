cmake_minimum_required(VERSION 3.10)

include(FindPkgConfig)

project(litebrowser CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -g")
set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_PREFIX_PATH "./lib")

set(LITEBROWSER_PATH src)

#pkg_check_modules(LB_LIBS REQUIRED fontconfig gdkmm-3.0 gtkmm-3.0 libcurl cairo)

set(SOURCE  ${LITEBROWSER_PATH}/App.cpp
            ${LITEBROWSER_PATH}/MainWindow.cpp
    )

set(HEADERS ${LITEBROWSER_PATH}/MainWindow.h
            ${LITEBROWSER_PATH}/App.h
    )

set(IMPORTED_LOCATION ./lib)

add_executable(litebrowser ${SOURCE} ${HEADERS})

include_directories(${LB_LIBS_INCLUDE_DIRS} /boot/home/config/non-packaged/include/litehtml /boot/system/develop/headers/private/netservices /boot/system/develop/headers/private/netservices2)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        C_STANDARD 99
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
        )

add_library(liblitehtml SHARED IMPORTED)

# TODO: use proper install path and lookup!
set_target_properties(liblitehtml PROPERTIES IMPORTED_LOCATION /boot/home/config/non-packaged/lib/liblitehtml.so )

target_link_libraries(litebrowser be localestub translation bnetapi network netservices2 liblitehtml)
