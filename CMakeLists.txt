cmake_minimum_required(VERSION 3.17)

# Use the CMakeLists.txt's parent-directory-name for the project's id/name
get_filename_component(PROJECT_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_ID ${PROJECT_ID})
project(${PROJECT_ID})

file(GLOB project_SRC *.cpp *.h)

# Include ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${project_SRC})

target_link_libraries(
    ${PROJECT_NAME}
    ${CURSES_LIBRARIES}
)