#pragma once
#include <iostream>

struct Course{
private:
    std::string CourseID;
    std::string CourseName;
public:
    void setID(const std::string& ID);
    const std::string getID();
    void setName(const std::string& Name);
    const std::string getName();
};