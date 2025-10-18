#include <SQL.hpp>
#include <filesystem>

DateTime addDays(DateTime& dt, int days);

SQL::SQL(){
    std::string basePath = std::filesystem::current_path().parent_path();
    std::string dbPath = basePath + "/" + filename; 
    std::cout << dbPath << std::endl;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if(rc == SQLITE_OK){
        std::cout << "Database connected\n";
    }
    else
        std::cout << "Could not connect\n";
    this->fillTimetable();
}
SQL::~SQL(){
    sqlite3_close(this->db);
}
User* SQL::checkAccount(std::string username, std::string password){
    int rc;
    sqlite3_stmt* stmt = nullptr;
    try{
        const std::string query = "SELECT username, name, password FROM Student WHERE username = ? AND password = ? ";
        rc=sqlite3_prepare_v2(this->db, query.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if(rc==SQLITE_ROW){
            std::string n_no = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            Student *student = new Student(n_no, password);
            student->setName(name);
            sqlite3_finalize(stmt);
            return student;
        }
        else{
            sqlite3_finalize(stmt);
        }
    }catch(std::exception &e){
        std::cout << e.what();
    }
    try{
        const std::string query = "SELECT username, name, password FROM Admin WHERE username = ? AND password = ? ";
        rc=sqlite3_prepare_v2(this->db, query.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        std::cout << "Searching Admins" << std::endl;
        if(rc==SQLITE_ROW){
            std::string ID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            Admin *admin = new Admin(ID, password);
            std::cout << name << std::endl;
            admin->setName(name);
            sqlite3_finalize(stmt);
            return admin;
        }
        else{
            sqlite3_finalize(stmt);
        }
    }catch(std::exception &e){
        std::cout << e.what();
    }
    try{
        const std::string query = "SELECT username, name, password FROM Lecturer WHERE username = ? AND password = ? ";
        rc=sqlite3_prepare_v2(this->db, query.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if(rc==SQLITE_ROW){
            std::string n_no = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            Tutor *tutor = new Tutor(n_no, password);
            tutor->setName(name);
            sqlite3_finalize(stmt);
            return tutor;
        }
        else{
            sqlite3_finalize(stmt);
        }
    }catch(std::exception &e){
        std::cout << e.what();
    }
    std::cout << "No record found" << std::endl;
    return nullptr;
}

void SQL::fillTimetable(){
    DateTime startDate = DateTime(START_DATE, START_TIME);    
    sqlite3_stmt* checkStmt;
    sqlite3_stmt* insertStmt;

    std::cout << "Working here 1" << std::endl;

    const char* checkSQL = "SELECT 1 FROM Timetable WHERE weekNo = ?";
    const char* insertSQL = "INSERT INTO Timetable (weekNo, startDate, endDate) VALUES (?, ?, ?)";

    if (sqlite3_prepare_v2(db, checkSQL, -1, &checkStmt, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statements\n";
        return;
    }

    DateTime currentStart = startDate;

    std::cout << "Working here 2" << std::endl;

    for (int week = 1; week <= 52; ++week) {
        sqlite3_bind_int(checkStmt, 1, week);
        int rc = sqlite3_step(checkStmt);

        if (rc != SQLITE_ROW) { // Week doesn't exist
            DateTime currentEnd = addDays(currentStart, 6);

            sqlite3_bind_int(insertStmt, 1, week);
            sqlite3_bind_text(insertStmt, 2, currentStart.getDate().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3, currentEnd.getDate().c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                std::cerr << "Failed to insert week " << week << ": " << sqlite3_errmsg(db) << "\n";
            }

            sqlite3_reset(insertStmt);
            sqlite3_clear_bindings(insertStmt);

            currentStart = addDays(currentStart, 7); // Move to next week
        } else {
            // Week exists, skip
            currentStart = addDays(currentStart, 7); // Still need to move forward
        }

        sqlite3_reset(checkStmt);
        sqlite3_clear_bindings(checkStmt);
    }

    sqlite3_finalize(checkStmt);
    sqlite3_finalize(insertStmt);
}

DateTime addDays(DateTime& original, int daysToAdd) {
    std::tm timeStruct = original.to_tm();
    timeStruct.tm_mday += daysToAdd;

    // Normalize the time
    std::mktime(&timeStruct);

    char buffer[11]; // "DD-MM-YYYY" + null terminator
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", &timeStruct);

    // Use original time or set to "00:00:00"
    std::string time = original.getTime().empty() ? "00:00:00" : original.getTime();
    return DateTime(buffer, time);
}

