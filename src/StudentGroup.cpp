#include <StudentGroup.hpp>

StudentGroup::StudentGroup(const std::string &groupID){
    this->GroupID = groupID;
}

void StudentGroup::addCourse(const std::string &courseID){
    this->Courses.push_back(courseID);
}

const std::string StudentGroup::getGroupID()
{
    return this->GroupID;
}

std::optional<std::string> StudentGroup::getCourseID()
{
    if(!this->Courses.empty()){
        std::string course = this->Courses.back();
        this->Courses.pop_back();
        return course;
    }else{
        return std::nullopt;
    }
}
