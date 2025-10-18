#pragma once
#include <iostream>
#include <User.hpp>
#include <unordered_map>
#include <functional>
#include <vector>
#include <StudentGroup.hpp>
#include <Module.hpp>
#include <Session.hpp>
#include <Room.hpp>
#include <Student.hpp>
#include <Tutor.hpp>

class Admin:public User{
private:
    std::unordered_map<int, std::function<void()>> menu;
    void mapCommands(); // Mapping commands to menu
    std::vector<std::string> logs;

    // Admin functions (Interactive methods)
    void add_module();
    void delete_module();
    void modify_module();
    void add_student_group();
    void delete_student_group();
    void modify_student_group();
    void define_sessions();
    void modify_sessions();
    void register_students();
    void assign_student_to_groups();
    void register_lecturers();
    void assign_lecturers_to_sessions();
    void create_timetable_for_week();
    void update_timetable_for_week();
    void search_for_timetable_conflicts();
    void register_course();
    void delete_course();

    //Connecting tables
    bool connect_stdgrp_to_course(StudentGroup& group);
    bool disconnect_stdgrp_to_course(StudentGroup &group);
    void connect_lec_to_module(Module &module);
    void disconnect_lec_to_module(Module& module);
    void connect_stdgrp_to_session(Session& session);
    void disconnect_stdgrp_to_session(Session& session);

    //Testable methods
    bool add_module_to_db(Module &module);
    bool delete_module_by_id(const std::string &id);
    bool modify_module_by_id(Module& module);
    bool add_stdgrp_to_db(StudentGroup &group);
    bool delete_stdgrp_from_db(StudentGroup &group);
    bool add_session_to_db(Session &session);
    bool modify_session_in_db(Session &session);
    bool add_student_to_db(Student &student);
    bool add_student_to_std_grp(Student &student);
    bool add_lecturer_to_db(Tutor &tutor);
    bool check_session_and_lec(const std::string &sessionID, const std::string &lecturerID);
    bool connect_lec_to_session(const std::string &sessionID, const std::string &lecturerID);
    bool create_timetable(Timetable &tt);
    const Session createSession();
    Timetable* retrieveTimetable(const int &week);
    void fetchTimetable(Timetable *tt);
    std::vector<std::string> fetchGroups(const std::string &sessionID);
    void delete_session(const std::string &sessionID);
    bool groupConflict();
    bool roomConflict();
    bool lecturerConflict();
    bool studentConflict();

public:
    Admin(const std::string &username, const std::string &password);
    void displayCommands() override;
    void execute(int command) override;  
    void push_action(const std::string& action);
    void export_log_to_txt();
};