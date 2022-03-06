set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(LMDB REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

add_subdirectory(src)
