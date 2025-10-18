#include <DateTime.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>

DateTime::DateTime()
{
}

DateTime::DateTime(const std::string &date, const std::string &time)
{
    this->date = date;
    this->time = time;
    // std::cout << "Date: " << date << ", Time: " << time << std::endl;
}

DateTime::~DateTime()
{
}

void DateTime::setDate(const int &day, const int &month, const int &year)
{
    if(day > 0 && day < 32
    && month > 0 && month < 13
    && year > 2023){
        std::ostringstream oss;
        switch(month){
            case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                oss << std::setw(2) << std::setfill('0') << day << "-" << std::setw(2) << std::setfill('0') << month << "-" << year;
                this->date = oss.str();
                break;
            case 2:
                if(day > 29)
                    std::cout << "February doesn't have more than 29 days in 2025-26 Academic year" << std::endl;
                else{
                    oss << std::setw(2) << std::setfill('0') << day << "-" << std::setw(2) << std::setfill('0') << month << "-" << year;
                    this->date = oss.str();
                }
                break;
            case 4: case 6: case 9: case 11:
                if(day > 30){
                    std::cout << "This month does not have more than 30 days"<< std::endl;
                }
                else{
                    oss << std::setw(2) << std::setfill('0') << day << "-" << std::setw(2) << std::setfill('0') << month << "-" << year;
                    this->date = oss.str();
                }
                break;
            default:
                break;
        }
    }
    else{
        std::cout << "Invalid input" << std::endl;
    }
}

const std::string DateTime::getDate(){
    return this->date;
}

void DateTime::setTime(const int &hour, const int &min, const int &second){
    if(hour > -1 && hour < 24
    && min > -1 && min < 60
    && second > -1 && min < 60){
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << min << ":" << std::setw(2) << std::setfill('0') << second;
        this->time = oss.str();
    }else{
        std::cout << "Invalid time, enter valid time in 24H format" << std::endl;
    }
}

const std::string DateTime::getTime()
{
    return this->time;
}

std::tm DateTime::to_tm() {
    std::tm t = {};
    try {
        t.tm_mday = std::stoi(this->date.substr(0, 2));  // Day
        t.tm_mon = std::stoi(this->date.substr(3, 2)) - 1;  // Month (0-11)
        t.tm_year = std::stoi(this->date.substr(6, 4)) - 1900;  // Year (since 1900)

        t.tm_hour = std::stoi(this->time.substr(0, 2));  // Hour
        t.tm_min = std::stoi(this->time.substr(3, 2));  // Minute
        t.tm_sec = std::stoi(this->time.substr(6, 2));  // Second
    } catch (const std::invalid_argument& e) {
        std::cout << "Error: Invalid date or time format!" << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Error: Date or time value out of range!" << std::endl;
    }

    return t;
}


const std::string DateTime::datetime()
{
    return this->date + " " + this->time;
}

const int DateTime::getWeek(){return this->week;}

void DateTime::calculateWeek() {
    std::tm t_current = this->to_tm();
    DateTime start = DateTime(START_DATE, START_TIME);
    std::tm t_start = start.to_tm();

    std::cout << "Start date: " << start.date << " " << start.time << std::endl;
    std::cout << "Current date: " << this->date << " " << this->time << std::endl;
    
    std::time_t start_date = mktime(&t_start);
    std::time_t current_date = mktime(&t_current);

    // Check if mktime failed
    if (start_date == -1 || current_date == -1) {
        std::cout << "Error: mktime failed!" << std::endl;
        return;
    }

    double secDiff = std::difftime(current_date, start_date);
    this->week = static_cast<int>(secDiff / (7 * 24 * 60 * 60)) + 1;

    std::cout << "Calculated week: " << this->week << std::endl;
}


std::string DateTime::getDayOfTheWeek() {
    std::tm tm = this->to_tm();
    std::mktime(&tm); // Normalize tm structure
    static const std::string days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    return days[tm.tm_wday];
}