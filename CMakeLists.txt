cmake_minimum_required(VERSION 3.0)
project(cpp_mirror)

set(CMAKE_CXX_STANDARD 17)

include_directories(cpp_mirror/headers/
                   /usr/local/Cellar/boost/1.72.0/include/)

file(GLOB PROJECT_HEADERS cpp_mirror/headers/*.hpp
                          cpp_mirror/headers/*.h)
file(GLOB PROJECT_SOURCES cpp_mirror/sources/*.cpp)

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

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")