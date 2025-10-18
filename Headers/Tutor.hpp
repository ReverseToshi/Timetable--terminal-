#pragma once
#include <iostream>
#include <User.hpp>
#include <unordered_map>
#include <functional>
#include <Timetable.hpp>
#include <chrono>
#include <ctime>
#include <Student.hpp>

class Tutor:public User{
private:
    void view_timetable();
    void view_timetable_by_week();
    void view_timetable_by_module();
    void view_timetable_for_student();
    void view_timetable_by_room();
    void displayWeeks();
    std::unordered_map<int, std::function<void()>> menu;
    void mapCommands();
    void fetchTimetable(Timetable* tt);
    void export_timetable(Timetable* tt);
    std::optional<Student> fetchStudent(const std::string &studentID);
    void fetchTimetable_by_module(Timetable *tt, const std::string &moduleID);
    void fetchTimetable_for_student(Timetable *tt, Student& student);
    void fetchTimetable_by_room(Timetable *tt, const std::string &roomID);

public:
    Tutor(const std::string &username, const std::string &password);
    void displayCommands() override;
    void execute(int command) override;
};
