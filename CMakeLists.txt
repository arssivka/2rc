cmake_minimum_required(VERSION 3.0)
project(rrc VERSION 0.0.2)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

option(RRC_BUILD_TESTS "Build tests." OFF)

add_subdirectory(lib)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SRCS
        include/rrc/abstract_launcher.h
        include/rrc/non_copyable.h
        include/rrc/topic_holder.h
        include/rrc/topic.h
        src/rrc/lockfree_launcher.cpp
        include/rrc/lockfree_launcher.h
        include/rrc/lockfree_task_queue.h
        include/rrc/concurrent_task_queue.h
        include/rrc/topic_mechanism.h
        include/rrc/service_holder.h
        include/rrc/service_mechanism.h
        include/rrc/rrc.h
        include/rrc/core.h
        include/rrc/mechanism.h
        src/rrc/task_scheduler.cpp
        include/rrc/task_scheduler.h
        include/rrc/task_queue.h
        include/rrc/core_base.h
        include/rrc/concurrent_priority_queue.h
        include/rrc/string.h
        include/rrc/elem_less.h
        include/rrc/result_code.h
        include/rrc/shared_function.h
        include/rrc/async_worker.h
        include/rrc/callback_defines.h
        src/rrc/string.cpp
        src/rrc/topic.cpp
        src/rrc/topic_holder.cpp
        src/rrc/service_mechanism.cpp
        src/rrc/service_holder.cpp
        src/rrc/core_base.cpp
        src/rrc/topic_mechanism.cpp
        include/rrc/notifier.h
        src/rrc/core.cpp
        )

configure_file("${PROJECT_SOURCE_DIR}/template/defines.h.in"
        "${PROJECT_BINARY_DIR}/defines.h")


add_library(rrc ${SRCS})
target_link_libraries(rrc concurrentqueue)
target_include_directories(rrc PUBLIC include)

if(RRC_BUILD_TESTS)
    add_subdirectory(test)
endif()