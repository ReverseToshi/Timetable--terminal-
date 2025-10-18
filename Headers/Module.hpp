#pragma once
#include <iostream>
#include <vector>
#include <optional>

class Module{
private:
    std::string ID;
    std::string Name;
    int year;
    std::vector<std::string> Lecturers;

public:
    Module(const std::string& moduleID, const std::string& moduleName, const int& year);
    const std::string getID();
    const std::string getName();
    const int getYear();
    void addLecturer(const std::string& lecturerID);
    std::optional<std::string> getLecturer();
};