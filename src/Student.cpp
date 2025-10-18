#include <Student.hpp>
#include <sqlite3.h>
#include <filesystem>
#include <DateTime.hpp>

inline std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%d-%m-%Y %H:%M:%S");
    return oss.str();
}

inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::stringstream ss(str);             // Create a stringstream object from the input string
    std::string item;                      // Temporary string to hold each extracted token
    std::vector<std::string> tokens;       // Vector to store the resulting substrings

    // Extract substrings separated by the delimiter and add them to the vector
    while (std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }

    return tokens;                         // Return the vector of tokens
}

void Student::view_timetable(){
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    if(rc!=SQLITE_OK){
        std::cout << "Failed to connect to DB" << std::endl;
        return;
    }
    std::string current_datetime = getCurrentTimeString();
    std::vector<std::string> current_tm = split(current_datetime, ' ');
    DateTime dt = DateTime(current_tm[0], current_tm[1]);
    dt.calculateWeek();
    int week = dt.getWeek();

    Timetable* tt = new Timetable(week);

    this->fetchTimetable(tt);

    tt->printTimetable();
    bool export_tt;
    std::cout << "Enter 1 if you want to export timetable, else enter 0:";
    std::cin >> export_tt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    if(!export_tt){
        this->export_timetable(tt);
    }

    delete tt;
    sqlite3_close(db);
}

void Student::view_timetable_by_week()
{
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    if(rc!=SQLITE_OK){
        std::cout << "Failed to connect to DB" << std::endl;
        return;
    }

    int week;
    std::cout << "Enter week number:";
    std::cin >> week;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    Timetable* tt = new Timetable(week);

    this->fetchTimetable(tt);
    tt->printTimetable();

    bool export_tt;
    std::cout << "Enter 1 if you want to export timetable, else enter 0:";
    std::cin >> export_tt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    if(!export_tt){
        this->export_timetable(tt);
    }
    
    delete tt;
    sqlite3_close(db);
}

void Student::view_timetable_by_module()
{
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    if(rc != SQLITE_OK) {
        std::cout << "Failed to connect to DB" << std::endl;
        return;
    }

    std::string current_datetime = getCurrentTimeString();
    std::vector<std::string> current_tm = split(current_datetime, ' ');
    DateTime dt(current_tm[0], current_tm[1]);
    dt.calculateWeek();
    int week = dt.getWeek();

    Timetable* tt = new Timetable(week);

    std::string moduleID;
    std::cout << "Enter Module ID:";
    std::getline(std::cin, moduleID);

    this->fetchTimetable_by_module(tt, moduleID);

    tt->printTimetable();

    bool export_tt;
    std::cout << "Enter 1 if you want to export timetable, else enter 0: ";
    std::cin >> export_tt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(!export_tt) {
        this->export_timetable(tt);
    }

    delete tt;
    sqlite3_close(db);
}

void Student::view_timetable_by_tutor()
{
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    if(rc != SQLITE_OK) {
        std::cout << "Failed to connect to DB" << std::endl;
        return;
    }

    std::string current_datetime = getCurrentTimeString();
    std::vector<std::string> current_tm = split(current_datetime, ' ');
    DateTime dt(current_tm[0], current_tm[1]);
    dt.calculateWeek();
    int week = dt.getWeek();

    Timetable* tt = new Timetable(week);

    std::string lecturerID;
    std::cout << "Enter Lecturer ID:";
    std::getline(std::cin, lecturerID);

    this->fetchTimetable_by_lecturer(tt, lecturerID);

    tt->printTimetable();

    bool export_tt;
    std::cout << "Enter 1 if you want to export timetable, else enter 0: ";
    std::cin >> export_tt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(!export_tt) {
        this->export_timetable(tt);
    }

    delete tt;
    sqlite3_close(db);
}

void Student::view_timetable_by_room()
{
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    if(rc != SQLITE_OK) {
        std::cout << "Failed to connect to DB" << std::endl;
        return;
    }

    std::string current_datetime = getCurrentTimeString();
    std::vector<std::string> current_tm = split(current_datetime, ' ');
    DateTime dt(current_tm[0], current_tm[1]);
    dt.calculateWeek();
    int week = dt.getWeek();

    Timetable* tt = new Timetable(week);

    std::string roomID;
    std::cout << "Enter Room ID:";
    std::getline(std::cin, roomID);

    this->fetchTimetable_by_room(tt, roomID);

    tt->printTimetable();

    bool export_tt;
    std::cout << "Enter 1 if you want to export timetable, else enter 0: ";
    std::cin >> export_tt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(!export_tt) {
        this->export_timetable(tt);
    }

    delete tt;
    sqlite3_close(db);
}

void Student::mapCommands(){
    menu[1]=std::bind(&Student::view_timetable, this);
    menu[2]=std::bind(&Student::view_timetable_by_week, this);
    menu[3]=std::bind(&Student::view_timetable_by_room, this);
    menu[4]=std::bind(&Student::view_timetable_by_tutor, this);
    menu[5]=std::bind(&Student::view_timetable_by_module, this);
    menu[6]=std::bind(&Student::displayWeeks, this);
}


Student::Student(const std::string &username) : User(username)
{
}

Student::Student(const std::string &username, const std::string &password) : User(username, password)
{
    this->mapCommands();
}

void Student::displayCommands(){
    std::cout << "Enter the corresponding number of the command you want to execute" << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "1. View this week's timetable" << std::endl;
    std::cout << "2. Search timetable by week" << std::endl;
    std::cout << "3. Search timetable by room" << std::endl;
    std::cout << "4. Search timetable by lecturer" << std::endl;
    std::cout << "5. Search timetable by module" << std::endl;
    std::cout << "6. Display weeks and their dates" << std::endl;
    std::cout << "Enter your choice:";
}

void Student::execute(int command){
    if(this->menu.count(command))
        menu[command]();
    else{
        std::cout << "Invalid command"<<std::endl;
    }
}

void Student::setCourse(const std::string& course){this->CourseID = course;}

const std::string Student::getCourse(){return this->CourseID;}

const std::optional<std::string> Student::getGroupID(){
    if(this->Groups.empty()){return std::nullopt;}
    std::string groupID = this->Groups.back();
    this->Groups.pop_back();
    return groupID;
}

void Student::addGroup(const std::string& groupID){
    this->Groups.push_back(groupID);
}

void Student::fetchTimetable(Timetable* tt){
    sqlite3_stmt* stmt;
    try{
        const char* query = R"(
        SELECT s.*
        FROM Student st
        JOIN Std_to_StdGrp sg ON st.username = sg.studentID
        JOIN StdGrp_to_Session gss ON sg.groupID = gss.groupID
        JOIN Session s ON gss.sessionID = s.sessionID
        WHERE st.username = ? AND s.weekNo = ?
        )";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        }else{
            sqlite3_bind_text(stmt, 1, this->getUsername().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, tt->getWeek());
            std::string sessionID;
            std::string sessionName;
            std::string moduleID;
            std::string lecturerID;
            std::string roomID;
            std::string scheduledAt;
            std::vector<std::string> dt;
            double duration;
            while(sqlite3_step(stmt)==SQLITE_ROW){
                sessionID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                sessionName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                moduleID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                std::string lecturerID = (sqlite3_column_type(stmt, 3) == SQLITE_NULL) ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                scheduledAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                dt = split(scheduledAt, ' ');
                roomID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                duration = sqlite3_column_double(stmt, 7);
                Session session = Session(sessionID, dt[0], dt[1], roomID);
                session.setDuration(duration);
                session.setLecturer(lecturerID);
                session.setModule(moduleID);
                session.setName(sessionName);

                tt->addSession(session);
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

void Student::export_timetable(Timetable *tt)
{
    std::string filename = "Timetable_week_"+std::to_string(tt->getWeek())+".csv";
    tt->exportCSV(filename);
}

void Student::fetchTimetable_by_module(Timetable* tt, const std::string& moduleID){
    std::string query = R"(
        SELECT s.*
        FROM Student st
        JOIN Std_to_StdGrp sg ON st.username = sg.studentID
        JOIN StdGrp_to_Session gss ON sg.groupID = gss.groupID
        JOIN Session s ON gss.sessionID = s.sessionID
        WHERE st.username = ? AND s.weekNo = ? AND s.moduleID = ?
    )";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, this->getUsername().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, moduleID.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, tt->getWeek());

        std::string sessionID;
        std::string sessionName;
        std::string moduleID;
        std::string lecturerID;
        std::string roomID;
        std::string scheduledAt;
        std::vector<std::string> dt;
        float duration;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            sessionID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            sessionName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            moduleID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            lecturerID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            scheduledAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            dt = split(scheduledAt, ' ');
            roomID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            duration = sqlite3_column_double(stmt, 7);
            Session session = Session(sessionID, dt[0], dt[1], roomID);
            session.setDuration(duration);
            session.setLecturer(lecturerID);
            session.setModule(moduleID);
            session.setName(sessionName);

            tt->addSession(session);
        }
    }

    sqlite3_finalize(stmt);
}

void Student::fetchTimetable_by_lecturer(Timetable* tt, const std::string& lecturerID){
    std::string query = R"(
        SELECT s.*
        FROM Student st
        JOIN Std_to_StdGrp sg ON st.username = sg.studentID
        JOIN StdGrp_to_Session gss ON sg.groupID = gss.groupID
        JOIN Session s ON gss.sessionID = s.sessionID
        WHERE st.username = ? AND s.weekNo = ? AND s.lecturerID = ?
    )";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, this->getUsername().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, lecturerID.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, tt->getWeek());

        std::string sessionID;
        std::string sessionName;
        std::string moduleID;
        std::string lecturerID;
        std::string roomID;
        std::string scheduledAt;
        std::vector<std::string> dt;
        float duration;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            sessionID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            sessionName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            moduleID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            lecturerID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            scheduledAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            dt = split(scheduledAt, ' ');
            roomID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            duration = sqlite3_column_double(stmt, 7);
            Session session = Session(sessionID, dt[0], dt[1], roomID);
            session.setDuration(duration);
            session.setLecturer(lecturerID);
            session.setModule(moduleID);
            session.setName(sessionName);

            tt->addSession(session);
        }
    }
}

void Student::fetchTimetable_by_room(Timetable* tt, const std::string& roomID){
    std::string query = R"(
        SELECT s.*
        FROM Student st
        JOIN Std_to_StdGrp sg ON st.username = sg.studentID
        JOIN StdGrp_to_Session gss ON sg.groupID = gss.groupID
        JOIN Session s ON gss.sessionID = s.sessionID
        WHERE st.username = ? AND s.weekNo = ? AND s.roomID = ?
    )";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, this->getUsername().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, roomID.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, tt->getWeek());

        std::string sessionID;
        std::string sessionName;
        std::string moduleID;
        std::string lecturerID;
        std::string roomID;
        std::string scheduledAt;
        std::vector<std::string> dt;
        float duration;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            sessionID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            sessionName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            moduleID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            lecturerID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            scheduledAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            dt = split(scheduledAt, ' ');
            roomID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            duration = sqlite3_column_double(stmt, 7);
            Session session = Session(sessionID, dt[0], dt[1], roomID);
            session.setDuration(duration);
            session.setLecturer(lecturerID);
            session.setModule(moduleID);
            session.setName(sessionName);

            tt->addSession(session);
        }
    }
}

void Student::displayWeeks()
{
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    if(rc != SQLITE_OK) {
        std::cout << "Failed to connect to DB" << std::endl;
        return;
    }
    
    std::cout << "Week\t\tStart Date\t\tEnd Date" << std::endl;
    sqlite3_stmt* stmt;
    const char* query = "SELECT * FROM Timetable";
    int week;
    std::string Start;
    std::string End;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){std::cout << "Failed to prepare statement:"<<sqlite3_errmsg(db) << std::endl; return;}
    while(sqlite3_step(stmt)==SQLITE_ROW){
        week = sqlite3_column_int(stmt,0);
        Start = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        End = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));

        std::cout << week << "\t\t" << Start << "\t\t" << End << std::endl; 
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
