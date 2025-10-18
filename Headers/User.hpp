#pragma once
#include <iostream>
#include <sqlite3.h>

//Abstract class
class User{
private:
    std::string Username;
    std::string Name;
    std::string Password;
    const char *filename = "database.db";
protected:
    sqlite3* db;
public:
    User(const std::string& username, std::string password);
    User(const std::string& username);
    ~User();
    virtual void logout();
    virtual void displayCommands();
    virtual void execute(int command);
    const std::string getUsername();
    const std::string getName();
    const std::string getPassword();
    void setName(const std::string& name);
    sqlite3* getDB();
    const std::string getFileName();
    void setUsername(const std::string& username);
};

