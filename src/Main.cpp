#include <Admin.hpp>
#include <User.hpp>
#include <Student.hpp>
#include <SQL.hpp>
#include <Tutor.hpp>
#include <vector>
#include <deque>
#include <list>
#include <iostream>
#include <limits>

int main() {
    SQL *ctor = new SQL();
    std::string username;
    std::string password;
    User* user = nullptr;
    bool quit = false;
    if(!quit){
        do{
            std::cout << "Enter 0 to login, and 1 to quit:";
            std::cin >> quit;
            if(quit){
                delete ctor;
                return 0;
            }
            std::cout << "Enter username:";
            std::cin >> username;
            std::cout << "Enter password:";
            std::cin >> password;
            user = ctor->checkAccount(username, password);
            if(user==nullptr){
                std::cout << "Invalid username and password, try again" << std::endl;
            }
        } while(user==nullptr && !quit);
        std::cout << "Welcome " << user->getName() << std::endl;
        while(!quit){
            int command;
            user->displayCommands();
            if(std::cin >> command){
                if(command == 0){
                    user->logout();
                    break;
                }else{
                    user->execute(command);
                }
            }else{
                std::cout << "Invalid input, Enter a valid command" << std::endl;
            }
        }
    }
    return 0;
}