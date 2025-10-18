#include <Module.hpp>

Module::Module(const std::string& moduleID, const std::string& moduleName, const int& year){
    this->ID = moduleID;
    this->Name = moduleName;
    this->year = year;
}

const std::string Module::getID(){
    return this->ID;
}

const std::string Module::getName(){
    return this->Name;
}

const int Module::getYear(){
    return this->year;
}

void Module::addLecturer(const std::string &lecturerID){
    this->Lecturers.push_back(lecturerID);
    std::cout << this->Lecturers.back() << std::endl;
}

std::optional<std::string> Module::getLecturer()
{
    if(this->Lecturers.empty()){
        return std::nullopt;
    }else{
        std::string lecturer = Lecturers.back();
        Lecturers.pop_back();
        return lecturer;
    }
}
