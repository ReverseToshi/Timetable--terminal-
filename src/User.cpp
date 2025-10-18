#include <User.hpp>
#include <filesystem>

User::User(const std::string& username, std::string password){
    this->Username = username;
    this->Password = password;
}

User::User(const std::string &username)
{
    this->Username = username;
}

const std::string User::getUsername(){
    return this->Username;
}

const std::string User::getPassword(){
    return this->Password;
}

User::~User(){
    sqlite3_close(this->db);
    delete this->filename;
}

void User::logout()
{
    this->~User();
}

void User::displayCommands(){
    
}

void User::execute(int command)
{
}

void User::setName(const std::string &name){
    this->Name = name;
}

sqlite3 *User::getDB()
{
    return this->db;
}

const std::string User::getFileName()
{
    std::string basePath = std::filesystem::current_path().parent_path();
    std::string file = basePath+"/"+this->filename;
    return file;
}

void User::setUsername(const std::string &username)
{
    this->Username = username;
}

const std::string User::getName(){
    return this->Name;
}