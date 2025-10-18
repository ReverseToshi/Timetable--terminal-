#pragma once
#include <User.hpp>
#include <sqlite3.h>
#include <Student.hpp>
#include <Admin.hpp>
#include <Tutor.hpp>

class SQL{
private:
    sqlite3 *db;
    const char *filename = "database.db";
public:
    SQL();
    ~SQL();
    User* checkAccount(std::string username, std::string password);
    void fillTimetable();
};
