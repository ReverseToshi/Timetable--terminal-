#pragma once
#include <Session.hpp>
#include <DateTime.hpp>
#include <set>
#include <optional>
#include <iterator>
#include <iomanip>

class Timetable{
private:
    int weekNo;
    DateTime Start;
    DateTime End;
    std::set<Session> Sessions;

public:
    Timetable(const int& week);
    const int getWeek();
    const DateTime getStart();
    const DateTime getEnd();
    void addSession(Session& session);
    void printTimetable();
    void exportCSV(const std::string &filename);
    void setStart(const DateTime &start);
    void setEnd(const DateTime& end);
    std::optional<Session> getSession();
};