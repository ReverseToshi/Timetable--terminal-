#pragma once
#include <iostream>
#include <vector>
#include <optional>

struct StudentGroup{
private:
    std::string GroupID;
    std::vector<std::string> Courses;

public:
    StudentGroup(const std::string& groupID);
    void addCourse(const std::string& courseID);
    const std::string getGroupID();
    std::optional<std::string> getCourseID();
};