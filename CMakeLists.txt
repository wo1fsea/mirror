cmake_minimum_required(VERSION 3.0)
project(mirror)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(BOOST_INCLUDE_PATH C:/boost_1_72_0)
endif()

if (APPLE)
    set(BOOST_INCLUDE_PATH /usr/local/Cellar/boost/1.72.0/include/)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
endif()

include_directories(mirror/headers/
                   ${BOOST_INCLUDE_PATH})

file(GLOB PROJECT_HEADERS mirror/headers/*.hpp
                          mirror/headers/*.h)
file(GLOB PROJECT_SOURCES mirror/sources/*.cpp)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("headers" FILES ${PROJECT_HEADERS})
source_group("sources" FILES ${PROJECT_SOURCES})

add_executable(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES})

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )

