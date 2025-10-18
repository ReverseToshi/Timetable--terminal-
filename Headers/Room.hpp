#pragma once
#include <iostream>

struct Room{
private:
    std::string RoomID;
    std::string Building;
public:
    Room(const std::string& room, const std::string& building);
    const std::string getRoom();
    const std::string getBuilding();
};