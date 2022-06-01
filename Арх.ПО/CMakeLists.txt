cmake_minimum_required(VERSION 2.5)

project(poco-http-server-lab4)

set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ${REDISCPP_FLAGS}")

find_package(Poco REQUIRED Foundation Util Net XML JSON DataMySQL Data)
find_package(Boost 1.67.0 REQUIRED COMPONENTS thread system iostreams filesystem program_options regex)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
find_package(JNI REQUIRED)

add_executable(poco-http-server-lab4
    src/handlers/Factory.cpp
    src/handlers/PersonRequest.cpp
    src/main.cpp
    src/Server.cpp
    src/writer/SimplePocoHandler.cpp
    database/PersonEntity.cpp
    config/config.cpp
)

target_link_libraries(poco-http-server-lab4
    PocoUtil
    PocoNet
    PocoXML
    PocoFoundation
    PocoJSON
    PocoData
    PocoDataMySQL
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    amqpcpp
)

add_executable(amqp_consume
    src/writer/amqp_consume.cpp
    src/writer/SimplePocoHandler.cpp
    database/PersonEntity.cpp
    config/config.cpp
)

target_link_libraries(amqp_consume
    PocoUtil
    PocoNet
    PocoXML
    PocoFoundation
    PocoJSON
    PocoData
    PocoDataMySQL
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    amqpcpp
)
