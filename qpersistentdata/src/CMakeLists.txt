add_library(qpersistentdata STATIC qpersistentdata.h qpersistentdata.cpp)
target_link_libraries(qpersistentdata PUBLIC Qt::Core ${LMDB_LIBRARIES})
target_include_directories(qpersistentdata PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
