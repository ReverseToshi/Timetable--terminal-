#pragma once
#include <iostream>
#include <DateTime.hpp>
#include <vector>
#include <optional>

class Session{
private:
    std::string ID;
    std::string Name;
    DateTime datetime;
    int week;
    std::string Module;
    std::string LecturerID;
    std::string roomID;
    std::vector<std::string> Groups;
    int calculateWeek();
    float Duration;

public:
    Session(const std::string& ID, const std::string& date, const std::string& time, const std::string& room);
    ~Session();
    const std::string getID();
    const std::string getModule();
    void setName(const std::string& name);
    void setModule(const std::string& ModuleID);
    void setLecturer(const std::string& tutor);
    bool operator<(const Session& other) const;
    DateTime getDateTime() const; 
    const std::string getName();
    const std::string getLecturer();
    const std::string getRoom();
    const int getWeek();
    void addGroup(const std::string& groupID);
    const std::optional<std::string> getGroup();
    void addGroup(std::vector<std::string> groups);
    void setDuration(const float& duration);
    const float getDuration();
    const std::vector<std::string> getGroups();
};
