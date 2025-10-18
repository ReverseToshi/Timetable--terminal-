#include <Course.hpp>

void Course::setID(const std::string &ID)
{
    this->CourseID = ID;
}

const std::string Course::getID()
{
    return this->CourseID;
}

void Course::setName(const std::string& Name){
    this->CourseName = Name;
}

const std::string Course::getName(){
    return this->CourseName;
}