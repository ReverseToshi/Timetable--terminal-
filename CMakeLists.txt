cmake_minimum_required(VERSION 3.18)

#Project initialisation
project(NTU_TimeTable)

add_executable(NTU_TimeTable src/Main.cpp 
    sqlite3/sqlite3.c
    src/Admin.cpp 
    src/User.cpp 
    src/Student.cpp 
    src/Tutor.cpp
    src/SQL.cpp
    src/Session.cpp
    src/Timetable.cpp
    src/DateTime.cpp 
    src/Module.cpp 
    src/Course.cpp
    src/StudentGroup.cpp
    src/Room.cpp
)

target_include_directories(NTU_TimeTable PRIVATE ${CMAKE_SOURCE_DIR}/Headers ${CMAKE_SOURCE_DIR}/sqlite3)