cmake_minimum_required(VERSION 3.7)

project(ormodule VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions ("-Wall -Wextra -Wpedantic -Weffc++ -Wconversion -Wunreachable-code -Wold-style-cast")

include(FindPkgConfig)

# project files
set(SOURCE_FILES
        brocker/IBrocker.h
        brocker/IBrocker.cpp
        brocker/IBrockerCallback.h
        brocker/DefaultBrocker.h
        brocker/DefaultBrocker.cpp
        brocker/MQTTBrocker.h
        brocker/MQTTBrocker.cpp
        database/IDatabase.h
        database/IDatabase.cpp
        database/IDatabaseCallback.h
        database/DefaultDB.h
        database/DefaultDB.cpp
        database/FileDB.h
        database/FileDB.cpp
        ORModule.cpp
        ormodulecallback.cpp
        ordebug.cpp
        orfile.cpp
        orstatus.cpp
        orutils.cpp
        ORModuleImpl.cpp
        DataBlock.cpp
        ORSynchronizer.cpp
        Record.cpp
        Subscription.cpp
        )

set(HEADER_FILES
        ORModule.h
        ormodulecallback.h
        ordebug.h
        orfile.h
        orstatus.h
        orutils.h
        DataBlock.h
        SimpleDataBlock.h
        ORModuleImpl.h
        Field.h
        ORSynchronizer.h
        Record.h
        Subscription.h
        URecord.h
        )

set(DATAHEADER_FILES
        datatypes/FileData.h
        datatypes/DataStats.h
        datatypes/DataBoundary.h
        datatypes/DataQuery.h
	    datatypes/DataTaho.h
	    datatypes/DataHit.h
	    datatypes/DataSpectr.h
        datatypes/DisplacementStat.h
        datatypes/VelocityStat.h
        datatypes/ModuleDescription.h
        )

#TODO: move to mysql adapter
pkg_check_modules(MYSQL QUIET mysqlclient)
if (MYSQL_FOUND)
    message("Library MySQL found, add MySQL support")
    add_definitions(-D_MYSQL_FOUND_=true)
    list(APPEND SOURCE_FILES
            database/MySQLDB.h
            database/MySQLDB.cpp
            )
else()
    message("Library MySQL not found, MySQL support disabled")
endif()

pkg_check_modules(CLICKHOUSE QUIET clickhouse-cpp)
if (CLICKHOUSE_FOUND)
    message("Library clickhouse-cpp found, add ClickHouse support")
    add_definitions(-D_CLICKHOUSE_FOUND_=true)
    list(APPEND SOURCE_FILES
            database/ClickhouseDB.h
            database/ClickhouseDB.cpp
            )
else()
    message("Library clickhouse-cpp not found, ClickHouse support disabled")
endif()

pkg_check_modules(MONGODB QUIET libmongocxx)
if(MONGODB_FOUND)
    message("Library mongocxx found, add MongoDB support")
    add_definitions(-D_MONGODB_FOUND_=true)
    list(APPEND SOURCE_FILES
            database/DBMongo.h
            database/DBMongo.cpp
            )
    # headers
    include_directories(${MONGODB_INCLUDE_DIRS})
else()
    message("Library mongocxx not found, MongoDB support disabled")
endif()

pkg_check_modules(SQLITE QUIET sqlite3)
if (SQLITE_FOUND)
    message("Library sqlite3 found, add SQlite support")
    add_definitions(-D_SQLITE_FOUND_=true)
    list(APPEND SOURCE_FILES
            database/SqliteDB.h
            database/SqliteDB.cpp
            )
else()
    message("Library sqlite3 not found, SQlite support disabled")
endif()

pkg_check_modules(CURL QUIET libcurl)
if (CURL_FOUND)
    message("Library libcurl found, add SystemAPI support")
    add_definitions(-D_CURL_FOUND_=true)
    list(APPEND SOURCE_FILES
            database/SystemAPIDB.h
            database/SystemAPIDB.cpp
            )
else()
    message("Library libcurl not found, SystemAPI support disabled")
endif()

# TARGET
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} ${DATA_HEADER} ormodule.pc ormodule.pc.in)

target_link_libraries(${PROJECT_NAME} mosquitto uv)

if (SQLITE_FOUND)
    target_link_libraries(${PROJECT_NAME} ${SQLITE_LIBRARIES})
endif()

if (MYSQL_FOUND)
    target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES})
endif()

if (CLICKHOUSE_FOUND)
    target_link_libraries(${PROJECT_NAME} ${CLICKHOUSE_LIBRARIES})
endif()

if (MONGODB_FOUND)
    target_link_libraries(${PROJECT_NAME} ${MONGODB_LIBRARIES})
endif()

if (CURL_FOUND)
    target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
endif()

# set lib version
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

#for pc file
set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
configure_file(ormodule.pc.in ormodule.pc @ONLY)
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

# installs
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/${PROJECT_NAME})
install(FILES ${DATAHEADER_FILES} DESTINATION include/${PROJECT_NAME}/datatypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ormodule.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")
#TODO: move it to optirepair package
#install(FILES optirepair DESTINATION ${CMAKE_INSTALL_PREFIX}/../etc/logrotate.d)
