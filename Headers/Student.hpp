#pragma once
#include <User.hpp>
#include <unordered_map>
#include <iostream>
#include <optional>
#include <functional>
#include <chrono>
#include <ctime>
#include <Timetable.hpp>
#include <Session.hpp>

class Student:public User{
private:
    std::string CourseID;
    void view_timetable();
    void view_timetable_by_week();
    void view_timetable_by_module();
    void view_timetable_by_tutor();
    void view_timetable_by_room();
    std::unordered_map<int, std::function<void()>> menu;
    void mapCommands();
    std::vector<std::string> Groups;
    void fetchTimetable(Timetable* tt);
    void export_timetable(Timetable* tt);
    void fetchTimetable_by_module(Timetable *tt, const std::string &moduleID);
    void fetchTimetable_by_lecturer(Timetable *tt, const std::string &lecturerID);
    void fetchTimetable_by_room(Timetable *tt, const std::string &roomID);
    void displayWeeks();

public:
    Student(const std::string& username);
    Student(const std::string &username, const std::string &password);
    void displayCommands() override;
    void execute(int command) override;
    void setCourse(const std::string& course);
    const std::string getCourse();
    const std::optional<std::string> getGroupID();
    void addGroup(const std::string& groupID);
};

