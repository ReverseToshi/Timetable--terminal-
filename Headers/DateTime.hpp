#ifndef DATETIME_HPP
#define DATETIME_HPP

#include <iostream>
#include <ctime>
#include <cmath>

#define START_DATE "23-09-2024"
#define START_TIME "07:00:00"

struct DateTime{
private:
    std::string date;
    std::string time;
    int week;
public:
    DateTime();
    DateTime(const std::string& date, const std::string& time);
    ~DateTime();
    void setDate(const int& day, const int& month, const int& year);
    const std::string getDate();
    void setTime(const int& hour, const int& min, const int& second);
    const std::string getTime();
    std::tm to_tm();
    const std::string datetime();
    const int getWeek();
    void calculateWeek();
    std::string getDayOfTheWeek();
};

#endif // DATETIME_HPP