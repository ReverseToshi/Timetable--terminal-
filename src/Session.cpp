#include <Session.hpp>
#include <SQL.hpp>

Session::Session(const std::string &ID, const std::string& date, const std::string& time, const std::string& room) : datetime(date, time)
{
    this->ID = ID;
    this->datetime.calculateWeek();
    this->week = this->datetime.getWeek();
    std::cout << week << std::endl;
    this->roomID= room;
}

Session::~Session()
{
}

const std::string Session::getID()
{
    return this->ID;
}

const std::string Session::getModule()
{
    return this->Module;
}

void Session::setModule(const std::string &ModuleID){
    this->Module = ModuleID;
}

void Session::setLecturer(const std::string &tutor){
    this->LecturerID = tutor;
}

bool Session::operator<(const Session &other) const
{
    if(this->ID == other.ID){return false;}
    std::tm current = this->getDateTime().to_tm();
    std::tm another = other.getDateTime().to_tm();

    std::time_t current_date = mktime(&current);
    std::time_t another_date = mktime(&another);

    double secDiff = std::difftime(current_date, another_date);
    
    if(secDiff < 0){
        return true;
    }
    return false;
}

DateTime Session::getDateTime() const
{
    return this->datetime;
}

void Session::setName(const std::string& name){this->Name = name;}

const std::string Session::getLecturer(){return this->LecturerID;}

const std::string Session::getRoom(){return this->roomID;}

const int Session::getWeek(){return this->week;}

void Session::addGroup(const std::string &groupID){this->Groups.push_back(groupID); std::cout << this->Groups.back() << std::endl;}

const std::optional<std::string> Session::getGroup()
{
    if(this->Groups.empty()){return std::nullopt;}
    std::string group = this->Groups.back();
    this->Groups.pop_back();
    return group;
}

void Session::addGroup(std::vector<std::string> groups){
    this->Groups.insert(this->Groups.end(), groups.begin(), groups.end());
}

void Session::setDuration(const float &duration){this->Duration = duration;}

const std::string Session::getName(){return this->Name;}

const float Session::getDuration(){return this->Duration;}

const std::vector<std::string> Session::getGroups()
{
    return this->Groups;
}
