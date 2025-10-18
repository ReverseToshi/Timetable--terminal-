#include <Room.hpp>

Room::Room(const std::string& room, const std::string& building){
    this->RoomID = room;
    this->Building = building;
}

const std::string Room::getRoom(){return this->RoomID;}

const std::string Room::getBuilding(){return this->Building;}