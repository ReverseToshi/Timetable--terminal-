#include <Admin.hpp>
#include <SQL.hpp>
#include <Module.hpp>
#include <limits>
#include <filesystem>
#include <fstream>
#include <Course.hpp>
#include <StudentGroup.hpp>

// Splits a string into a vector of substrings based on a specified delimiter character
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


Admin::Admin(const std::string &username, const std::string &password) : User(username, password){
    this->mapCommands();
}

// Displays the commands the user can do
void Admin::displayCommands() {
    std::cout << std::endl;
    std::cout << "Enter the corresponding number of the command you want to execute" << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "1. Add module" << std::endl;
    std::cout << "2. Delete module" << std::endl;
    std::cout << "3. Modify module" << std::endl;
    std::cout << "4. Add student group" << std::endl;
    std::cout << "5. Delete student group" << std::endl;
    std::cout << "6. Modify student group" << std::endl;
    std::cout << "7. Define session" << std::endl;
    std::cout << "8. Modify session" << std::endl;
    std::cout << "9. Register student" << std::endl;
    std::cout << "10. Register course" << std::endl;
    std::cout << "11. Delete course" << std::endl;
    std::cout << "12. Assign student to group" << std::endl;
    std::cout << "13. Register lecturer" << std::endl;
    std::cout << "14. Assign lecturer to session" << std::endl;
    std::cout << "15. Create timetable for each week" << std::endl;
    std::cout << "16. Update timetable for each week" << std::endl;
    std::cout << "17. Search for timetable conflicts" << std::endl;
    std::cout << "Enter your choice: ";
}

void Admin::execute(int command){
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(this->menu.count(command)){
        menu[command]();
    }else{
        std::cout << "Invalid command" << std::endl;
    }
}

void Admin::push_action(const std::string &action)
{
    this->logs.push_back(action);
    if(this->logs.size()==3){
        this->export_log_to_txt();
        this->logs.clear();
    }
}

void Admin::export_log_to_txt(){
    std::string basePath = std::filesystem::current_path().parent_path();
    std::string filePath = basePath+"/"+"logs.txt";
    std::ofstream outFile(filePath, std::ios::app);
    
    if (!outFile) {
        std::cerr << "Failed to open the file: " << filePath << std::endl;
        return;
    }

    for (const auto& line : this->logs) {
        outFile << line << std::endl;
    }

    outFile.close();
}

// Maps integer menu options to corresponding Admin class methods using std::bind.
// This allows for dynamic invocation of methods based on user input (e.g., from a menu).
void Admin::mapCommands(){
    // Assign function pointers to each menu index using std::bind.
    // 'this' is captured so member functions can be called on the current Admin instance.
    menu[1] = std::bind(&Admin::add_module, this);                        // Add a new module
    menu[2] = std::bind(&Admin::delete_module, this);                    // Delete an existing module
    menu[3] = std::bind(&Admin::modify_module, this);                    // Modify module details
    menu[4] = std::bind(&Admin::add_student_group, this);                // Add a new student group
    menu[5] = std::bind(&Admin::delete_student_group, this);             // Delete a student group
    menu[6] = std::bind(&Admin::modify_student_group, this);             // Modify student group details
    menu[7] = std::bind(&Admin::define_sessions, this);                  // Define new sessions
    menu[8] = std::bind(&Admin::modify_sessions, this);                  // Modify session details
    menu[9] = std::bind(&Admin::register_students, this);                // Register new students
    menu[10] = std::bind(&Admin::register_course, this);                 // Register a new course
    menu[11] = std::bind(&Admin::delete_course, this);                   // Delete an existing course
    menu[12] = std::bind(&Admin::assign_student_to_groups, this);        // Assign students to groups
    menu[13] = std::bind(&Admin::register_lecturers, this);              // Register lecturers
    menu[14] = std::bind(&Admin::assign_lecturers_to_sessions, this);    // Assign lecturers to sessions
    menu[15] = std::bind(&Admin::create_timetable_for_week, this);       // Generate timetable for a week
    menu[16] = std::bind(&Admin::update_timetable_for_week, this);       // Update existing timetable
    menu[17] = std::bind(&Admin::search_for_timetable_conflicts, this);  // Check timetable for conflicts
}

void Admin::add_module() {
    std::string id, name, lecturer;
    int year;

    std::cout << "Enter Module ID: ";
    std::getline(std::cin, id);

    std::cout << "Enter Module Name: ";
    std::getline(std::cin, name);

    std::cout << "Enter Module year: ";
    std::cin >> year;

    // Clear leftover newline from input buffer to avoid skipping next getline
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Module new_module(id, name, year);

    std::cout << "Enter Lecturer ID to add (press Enter to finish adding):";
    while (true) {
        std::getline(std::cin, lecturer);
        if (lecturer.empty()) break;
        new_module.addLecturer(lecturer);
        std::cout << "Enter Lecturer ID to add (press Enter to finish adding):";
    }

    if (add_module_to_db(new_module)) {
        std::cout << "Successfully added module.\n";
    } else {
        std::cout << "Failed to add module.\n";
    }
    std::cout << std::endl;
}

void Admin::delete_module() {
    std::string id;
    std::cout << "Enter Module ID to delete: ";
    std::getline(std::cin, id);

    if(!this->delete_module_by_id(id)){std::cout << "Failed to delete module" << std::endl << std::endl;}
}

void Admin::modify_module() {
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for ensuring referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    std::string id;
    std::cout << "Enter Module ID to modify: ";
    std::getline(std::cin, id);

    sqlite3_stmt* stmt;
    const char* sel_que = "SELECT * FROM Module WHERE moduleID = ?";
    
    // Prepare SQL statement to find the module by ID
    if (sqlite3_prepare_v2(this->db, sel_que, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind the entered module ID to the prepared statement
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    // Check if a matching module record was found
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Module ID " << id << " not found.\n";
        sqlite3_finalize(stmt);
        return;
    }

    // Retrieve current name and year from result row
    std::string currentName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    int currentYear = sqlite3_column_int(stmt, 2);
    sqlite3_finalize(stmt);

    std::cout << "Current Name: " << currentName << ", Current Year: " << currentYear << "\n";

    std::cout << "Enter new name: ";
    std::string newName;
    std::getline(std::cin, newName);

    std::cout << "Enter new year: ";
    int newYear;
    std::cin >> newYear;

    // Clear input buffer in case of leftover newline
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Module module = Module(id, newName, newYear);
    
    // Update the module with the new values
    this->modify_module_by_id(module);
    std::cout << std::endl;
    sqlite3_close(db);
}

void Admin::add_student_group(){
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);

    // Ensure foreign key constraints are enforced
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    std::string groupID;
    std::string courseID;

    std::cout << "Enter Group ID: ";
    std::getline(std::cin, groupID);

    StudentGroup group = StudentGroup(groupID);

    // Adding courses for a group
    std::cout << "Enter Course ID to add (press Enter to finish adding):";
    while(true){
        std::getline(std::cin, courseID);
        if(courseID.empty())
            break;
        else{
            group.addCourse(courseID);
        }
        std::cout << "Enter Course ID to add (press Enter to finish adding):";
    }

    if(rc==SQLITE_OK){
        // Try adding the group and its courses to the database
        bool success = this->add_stdgrp_to_db(group);
        if(success){
            std::cout << "Successfully added Student Group " << group.getGroupID() << std::endl;
        }
    }else{
        std::cout << "Failed to connect to database" << std::endl;
    }
    std::cout << std::endl;
    sqlite3_close(db);
}

void Admin::delete_student_group(){
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);

    // Ensure foreign key constraints are enforced
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    std::string groupID;
    std::cout << "Enter Group ID to delete: ";
    std::getline(std::cin, groupID);

    // Create the student group object
    StudentGroup group = StudentGroup(groupID);

    if(rc==SQLITE_OK){
        // Try deleting the student group from the database
        bool success = this->delete_stdgrp_from_db(group);
        if(!success){
            std::cout << "Failed to delete student group " << group.getGroupID() << std::endl;
        }
    }else{
        std::cout << "Failed to connect to database" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::modify_student_group(){
    // Get the database file path
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);

    // Ensure foreign key constraints are enabled
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    std::string groupID;
    std::cout << "Enter Group ID: ";
    std::getline(std::cin, groupID);

    int choice;
    std::cout << "Enter 1 if you want to add a course connection, or 2 if you want to remove a course connection:";
    std::cin >> choice;  // Choice to add or remove course connections
    
    // Create the student group object
    auto group = StudentGroup(groupID);

    if(rc == SQLITE_OK){
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear the input buffer

        switch(choice){
            case 1:{
                // Add course connection
                std::string courseID;
                std::cout << "Enter Course ID to add (press Enter to finish adding):";
                while(true){
                    std::getline(std::cin, courseID);  // Get the course ID from the user
                    if(courseID.empty())
                        break;  // Exit loop when input is empty
                    else{
                        group.addCourse(courseID);  // Add the course to the group
                    }
                    std::cout << "Enter Course ID to add (press Enter to finish adding):";
                }
                // Connect the student group to the course
                this->connect_stdgrp_to_course(group);
                break;
            }
            case 2:{
                // Remove course connection
                std::string courseID;
                while(true){
                    std::getline(std::cin, courseID);  // Get the course ID from the user
                    if(courseID.empty())
                        break;  // Exit loop when input is empty
                    else{
                        group.addCourse(courseID);  // In this case, adding the course to the list for removal
                    }
                    std::cout << "Enter Course ID to remove (press Enter to finish adding):";
                }
                // Disconnect the student group from the course
                this->disconnect_stdgrp_to_course(group);
                break;
            }
            default:{
                // Handle invalid input
                std::cout << "Wrong input" << std::endl;
                break;
            }
        }
    }else{
        std::cout << "Failed to connect to the DB" << std::endl;  // Connection error message
    }
}

void Admin::define_sessions(){
    // Get the database file path
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);

    // Ensure foreign key constraints are enabled
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Create a session using the helper function to collect the session details
    Session session = this->createSession();

    // Check if the database connection was successful
    if(rc == SQLITE_OK){
        // Attempt to add the session to the database
        if(!this->add_session_to_db(session)){
            // If adding the session fails, display an error message
            std::cout << "Failed to add session: " << sqlite3_errmsg(db) << std::endl;
        }
    } else {
        // If the database connection fails, display an error message
        std::cout << "Failed to connect to database" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::modify_sessions(){
    // Open the database
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    std::string id;
    std::cout << "Enter Session ID to modify: ";
    std::getline(std::cin, id);

    // Check if the database connection is successful
    if(rc == SQLITE_OK){
        sqlite3_stmt* select_stmt;

        try {
            const char* sel_que = "SELECT * FROM Session WHERE SessionID = ?";
            // Prepare the SQL query to fetch the session data
            if(sqlite3_prepare_v2(this->db, sel_que, -1, &select_stmt, nullptr) != SQLITE_OK){
                std::cout << "Failed to prepare statement\n" << sqlite3_errmsg(db) << std::endl;
            } else {
                // Bind the session ID to the query
                sqlite3_bind_text(select_stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

                // Check if the session exists in the database
                if(sqlite3_step(select_stmt) != SQLITE_ROW){
                    std::cout << "Failed to find Module " << id << "\n" << sqlite3_errmsg(db) << std::endl;
                } else {
                    // Extract current session details from the database
                    std::string sessionID = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 0));
                    std::string sessionName = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 1));
                    std::string moduleID = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 2));
                    std::string lecturerID = (sqlite3_column_type(select_stmt, 3) == SQLITE_NULL) ? "" : reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 3));
                    std::string scheduledAt = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 4));
                    int weekNo = sqlite3_column_int(select_stmt, 5);
                    std::string roomID = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 6));

                    // Display the current session details
                    std::cout << "Session ID: " << sessionID << ", Session Name: " << sessionName << ", Session Room: " << roomID << std::endl;
                    std::cout << "Session lecturer: " << lecturerID << ", Session time: " << scheduledAt << ", Module ID: " << moduleID << std::endl << std::endl;
                    std::cout << "Enter new details" << std::endl;

                    // Prompt the user to input new session details
                    Session session = this->createSession();

                    // Attempt to update the session in the database
                    if(!this->modify_session_in_db(session)){
                        std::cout << "Failed to modify session " << session.getID() << std::endl;
                    }
                }
            }
        } catch(std::exception& e) {
            // Handle any exceptions that occur during execution
            std::cout << e.what() << std::endl;
        }

        // Finalize the prepared statement to release resources
        sqlite3_finalize(select_stmt);
    } else {
        // If the database connection fails, display an error message
        std::cout << "Failed to connect to database" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::register_students(){
    // Get the path to the database file and open the connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Declare variables to hold user input
    std::string username;
    std::string password;
    std::string studentName;
    std::string courseID;

    // Prompt the user for student details
    std::cout << "Enter Student ID: ";
    std::getline(std::cin, username);

    std::cout << "Enter Student Name: ";
    std::getline(std::cin, studentName);

    std::cout << "Enter Password: ";
    std::getline(std::cin, password);

    std::cout << "Enter Course ID: ";
    std::getline(std::cin, courseID);

    // Create a new Student object with the provided details
    Student student = Student(username, password);
    student.setName(studentName);
    student.setCourse(courseID);

    // Check if the database connection is successful
    if(rc == SQLITE_OK){
        // Attempt to add the student to the database
        if(!this->add_student_to_db(student)){
            std::cout << "Failed to register student" << std::endl;
        }
    } else {
        // If database connection fails, print an error message
        std::cout << "Failed to connect to database" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::assign_student_to_groups(){
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Declare variable to hold the student ID entered by the user
    std::string studentID;
    std::cout << "Enter Student ID:";
    std::getline(std::cin, studentID);

    // Check if the database connection was successful
    if(rc == SQLITE_OK){
        sqlite3_stmt* check_stmt;
        try{
            // Query to check if the student exists in the database
            const char* check_query = "SELECT username FROM Student WHERE username = ?";
            if(sqlite3_prepare_v2(db, check_query, -1, &check_stmt, nullptr) != SQLITE_OK){
                std::cout << "Failed to prepare statement" << sqlite3_errmsg(db) << std::endl;
            } else {
                // Bind the student ID to the SQL query
                sqlite3_bind_text(check_stmt, 1, studentID.c_str(), -1, SQLITE_TRANSIENT);
                
                // Execute the query and check if the student record exists
                if(sqlite3_step(check_stmt) != SQLITE_ROW){
                    std::cout << "No record found for Student ID: " << studentID << std::endl;
                } else {
                    // Create a new Student object if the record exists
                    Student student = Student(studentID);
                    std::string groupID;
                    
                    // Prompt the user to assign the student to one or more groups
                    std::cout << "Enter Student Group ID to assign to (Press enter to finish adding):";
                    while(true){
                        std::getline(std::cin, groupID);
                        if(groupID.empty()){break;}  // Break the loop when the user presses Enter without input
                        student.addGroup(groupID);  // Add the group ID to the student's list of groups
                    }

                    // Attempt to assign the student to the selected groups
                    if(!this->add_student_to_std_grp(student)){
                        std::cout << "Failed to assign student to the groups" << std::endl;
                    }
                }
            }
        } catch(std::exception& e){
            // Catch and print any exceptions that occur
            std::cout << e.what() << std::endl;
        }
        
        // Finalize the prepared statement to release resources
        sqlite3_finalize(check_stmt);
    } else {
        // Print an error message if the database connection fails
        std::cout << "Failed to connect to DB" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::register_lecturers(){
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Check if the database connection was successful
    if(rc == SQLITE_OK){
        // Declare and prompt the user for the Lecturer's ID
        std::string LecturerID;
        std::cout << "Enter Lecturer ID: ";
        std::getline(std::cin, LecturerID);

        // Declare and prompt the user for the Lecturer's password
        std::string Password;
        std::cout << "Enter Password: ";
        std::getline(std::cin, Password);

        // Declare and prompt the user for the Lecturer's name
        std::string Name;
        std::cout << "Enter Name: ";
        std::getline(std::cin, Name);

        // Create a new Tutor object using the provided details
        Tutor lecturer = Tutor(LecturerID, Password);
        lecturer.setName(Name);

        // Attempt to add the lecturer to the database
        if(!this->add_lecturer_to_db(lecturer)){
            std::cout << "Failed to register Lecturer" << std::endl;
        }
    } else {
        // Print an error message if the database connection fails
        std::cout << "Failed to connect to DB" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::assign_lecturers_to_sessions(){
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for maintaining referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Declare variables for the session ID and lecturer ID
    std::string SessionID;
    std::string LecturerID;
    
    // Prompt user for the session ID
    std::cout << "Enter Session ID: ";
    std::getline(std::cin, SessionID);

    // Prompt user for the lecturer ID
    std::cout << "Enter Lecturer ID: ";
    std::getline(std::cin, LecturerID);

    // Check if the database connection was successful
    if(rc == SQLITE_OK){
        // Verify if both session and lecturer exist in the database
        if(!this->check_session_and_lec(SessionID, LecturerID)){
            std::cout << "No records found" << std::endl;
        } else {
            // Attempt to assign the lecturer to the session
            if(!this->connect_lec_to_session(SessionID, LecturerID)){
                // Print an error message if the assignment fails
                std::cout << "Failed to assign lecturer to session\n" << sqlite3_errmsg(db) << std::endl;
            }
        }
    } else {
        // Print an error message if the database connection fails
        std::cout << "Failed to connect to DB" << std::endl;
    }
    std::cout << std::endl;
}

void Admin::create_timetable_for_week(){
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for ensuring referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Declare the week variable and prompt the user for input
    int week;
    std::cout << "Enter Week No. (1-52):";
    std::cin >> week;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    // Validate if the week number is within the valid range (1-52)
    if(week > 0 && week < 53){
        // Create a Timetable object for the specified week
        Timetable timetable = Timetable(week);

        // Check if the database connection was successful
        if(rc == SQLITE_OK){
            // Attempt to create the timetable for the given week
            if(!this->create_timetable(timetable)){
                // Print error message if creation fails
                std::cout << "Failed to create timetable" << std::endl;
            } else {
                // Print success message and log the action
                std::cout << "Successfully created Timetable for week " << timetable.getWeek() << std::endl;
                this->push_action("Created timetable for week " + std::to_string(timetable.getWeek()));
            }
        } else {
            // Print error message if the database connection fails
            std::cout << "Failed to connect to database" << std::endl;
        }
    } else {
        // Print an error message if the input week number is invalid
        std::cout << "Invalid input" << std::endl << std::endl;
    }
    std::cout << std::endl;
}

void Admin::update_timetable_for_week(){
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for ensuring referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    int week;
    std::cout << "Enter Week No. (1-52):";
    std::cin >> week;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    Timetable* tt = this->retrieveTimetable(week);
    tt->printTimetable();
    int choice;
    bool updating = true;
    while(updating){
        std::cout << "Enter 1 to add a session, 2 to modify a session, and 3 to delete a session:";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

        switch(choice){
            case 1:{
                this->define_sessions();
                break;
            }case 2:{
                this->modify_sessions();
                break;
            }case 3:{
                std::string sessionID;
                std::cout << "Enter ID to delete: ";
                std::getline(std::cin, sessionID);
                this->delete_session(sessionID);
                break;
            }default:{
                updating = false;
                break;
            }
        }
    }
    std::cout << "Updated timetable for week " << week << std::endl;
    this->push_action("Updated timetable for week: "+week);
    sqlite3_close(db);
}

void Admin::search_for_timetable_conflicts(){
    // Retrieve the database file path and open the database connection
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    
    // Enable foreign key constraints for maintaining referential integrity
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    if(this->studentConflict()){}
    if(this->lecturerConflict()){}
    if(this->groupConflict()){}
    if(this->roomConflict()){}
}

//done

void Admin::register_course(){
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    std::string courseID;
    std::cout << "Enter Course ID: ";
    std::getline(std::cin, courseID);

    std::string courseName;
    std::cout << "Enter Course Name: ";
    std::getline(std::cin, courseName);

    auto course = Course();
    course.setName(courseName);
    course.setID(courseID);

    if(rc==SQLITE_OK){
        sqlite3_stmt* stmt;
        try{
            const char* query = "INSERT INTO Course(courseID, courseName) VALUES(?,?)";
            if(sqlite3_prepare_v2(this->getDB(), query, -1, &stmt, nullptr)!=SQLITE_OK){
                std::cout << "Failed to prepare statement\n"<< sqlite3_errmsg(db) << std::endl;
            }else{
                sqlite3_bind_text(stmt, 1, course.getID().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, course.getName().c_str(), -1, SQLITE_TRANSIENT);

                if(sqlite3_step(stmt)==SQLITE_DONE){
                    std::cout << "Successfully added " << course.getID() << ":" << course.getName() << " course" << std::endl;
                    std::string log = "Added Course: " + course.getID() + " " + course.getName();;
                    this->push_action(log);
                }else{
                    std::cout << "Failed to add module\n" << sqlite3_errmsg(db) << std::endl;
                }
            }
        }catch(std::exception& e){
            std::cout << e.what() << std::endl;
        }
        sqlite3_finalize(stmt);
    }else{
        std::cout << "Failed to connect to database" << std::endl;
    }
}

void Admin::delete_course(){
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    std::string id;
    std::cout << "Enter Course ID to delete: ";
    std::getline(std::cin, id);
    if(rc==SQLITE_OK){
        sqlite3_stmt* stmt;
        try{
            const char* del_que = "DELETE FROM Course WHERE courseID = ?";
            if(sqlite3_prepare_v2(this->db, del_que, -1, &stmt, nullptr)!=SQLITE_OK){
                std::cout << "Failed to prepare statement\n" << sqlite3_errmsg(db) << std::endl;
            }else{
                sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
                if(sqlite3_step(stmt)!=SQLITE_DONE){
                    std::cout << "Failed to delete Course " << id << "\n" << sqlite3_errmsg(db) << std::endl;
                }else{
                    std::cout << "Successfully deleted Course " << id << std::endl;
                    std::string log = "Deleted Course: " + id;
                    this->push_action(log);
                }
            }
        }catch(std::exception& e){
            std::cout << e.what() << std::endl;
        }
        sqlite3_finalize(stmt);
    }
}

bool Admin::connect_stdgrp_to_course(StudentGroup& group){
    sqlite3_stmt* grp_crs_stmt;
    try{
        const char* grp_crs_que = "INSERT INTO StdGrp_to_Course(groupID, courseID) VALUES(?,?)";
        if(sqlite3_prepare_v2(this->db, grp_crs_que, -1, &grp_crs_stmt, nullptr)==SQLITE_OK){
            std::optional<std::string> course;
            std::string log;
            while(true){
                course = group.getCourseID();
                if(!course){
                    std::cout << "Finished connecting group with courses\n";
                    break;
                }else{
                    sqlite3_bind_text(grp_crs_stmt, 1, group.getGroupID().c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(grp_crs_stmt, 2, course->c_str(), -1, SQLITE_TRANSIENT);
                    if(sqlite3_step(grp_crs_stmt)!=SQLITE_DONE){
                        std::cout << "Failed to connect Group " << group.getGroupID() << " with course " << *course << std::endl;
                        std::cout << sqlite3_errmsg(db) << std::endl;
                    }else{
                        log = "Connected Group " + group.getGroupID() + " with course " + *course;
                        this->push_action(log);
                    }
                    sqlite3_reset(grp_crs_stmt);
                    sqlite3_clear_bindings(grp_crs_stmt);
                }
            }
        }else{
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    sqlite3_finalize(grp_crs_stmt);
    return true;
}

bool Admin::disconnect_stdgrp_to_course(StudentGroup& group){
    sqlite3_stmt* stmt;
    try{
        const char* del_que = "DELETE FROM StdGrp_to_Course WHERE groupID = ? AND courseID = ?";
        if(sqlite3_prepare_v2(db, del_que, -1, &stmt, nullptr)==SQLITE_OK){
            std::optional<std::string> course;
            std::string log;
            while(true){
                course = group.getCourseID();
                if(!course){
                    std::cout << "Finished disconnecting group from courses\n";
                    break;
                }else{
                    sqlite3_bind_text(stmt, 1, group.getGroupID().c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(stmt, 2, course->c_str(), -1, SQLITE_TRANSIENT);
                    if(sqlite3_step(stmt)!=SQLITE_DONE){
                        std::cout << "Failed to disconnect Group " << group.getGroupID() << " from course " << *course << std::endl;
                        std::cout << sqlite3_errmsg(db) << std::endl;
                    }else{
                        log = "Disconnected Group " + group.getGroupID() + " from course " + *course;
                        this->push_action(log);
                    }
                    sqlite3_reset(stmt);
                    sqlite3_clear_bindings(stmt);
                }
            }
        }else{
            return false;
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

void Admin::connect_lec_to_module(Module& module){
    sqlite3_stmt* lec_mod_stmt;
    std::string lecturerID;
    while(true){
        std::cout << "Enter the Lecturer ID for this module (press Enter to finish adding):";
        std::getline(std::cin, lecturerID);
        if(lecturerID.empty()){
            break;
        }
        module.addLecturer(lecturerID);
    }
    try{
        const char* lec_mod_que = "INSERT INTO Lecturer_to_Module(lecturerID, moduleID) VALUES(?,?)";
        if(sqlite3_prepare_v2(this->db, lec_mod_que, -1, &lec_mod_stmt, nullptr)==SQLITE_OK){
            std::optional<std::string> lecturer;
            std::string log;
            while(true){
                lecturer = module.getLecturer();
                if(!lecturer){
                    std::cout << "Finished connecting module with lecturers\n";
                    break;
                }else{
                    sqlite3_bind_text(lec_mod_stmt, 1, lecturer->c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(lec_mod_stmt, 2, module.getID().c_str(), -1, SQLITE_TRANSIENT);
                    if(sqlite3_step(lec_mod_stmt)!=SQLITE_DONE){
                        std::cout << "Failed to connect Module " << module.getID() << " with Lecturer " << *lecturer << std::endl;
                        std::cout << sqlite3_errmsg(db) << std::endl;
                    }else{
                        log = "Connected Module " + module.getID() + " with Lecturer " + *lecturer;
                        this->push_action(log);
                    }
                    sqlite3_reset(lec_mod_stmt);
                    sqlite3_clear_bindings(lec_mod_stmt);
                }
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
    sqlite3_finalize(lec_mod_stmt);
}

void Admin::disconnect_lec_to_module(Module& module){
    sqlite3_stmt* lec_mod_stmt;
    std::string lecturerID;
    while(true){
        std::cout << "Enter the Lecturer ID for this module (press Enter to finish adding):";
        std::getline(std::cin, lecturerID);
        if(lecturerID.empty()){
            break;
        }
        module.addLecturer(lecturerID);
    }
    try{
        const char* lec_mod_que = "DELETE FROM Lecturer_to_Module(lecturerID, moduleID) WHERE lecturerID = ? AND moduleID = ?";
        if(sqlite3_prepare_v2(this->db, lec_mod_que, -1, &lec_mod_stmt, nullptr)==SQLITE_OK){
            std::optional<std::string> lecturer;
            std::string log;
            while(true){
                lecturer = module.getLecturer();
                if(!lecturer){
                    std::cout << "Finished connecting module with lecturers\n";
                    break;
                }else{
                    sqlite3_bind_text(lec_mod_stmt, 1, lecturer->c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(lec_mod_stmt, 2, module.getID().c_str(), -1, SQLITE_TRANSIENT);
                    if(sqlite3_step(lec_mod_stmt)!=SQLITE_DONE){
                        std::cout << "Failed to disconnect Module " << module.getID() << " from Lecturer " << *lecturer << std::endl;
                        std::cout << sqlite3_errmsg(db) << std::endl;
                    }else{
                        log = "Disconnected Module " + module.getID() + " from Lecturer " + *lecturer;
                        this->push_action(log);
                    }
                    sqlite3_reset(lec_mod_stmt);
                    sqlite3_clear_bindings(lec_mod_stmt);
                }
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
    sqlite3_finalize(lec_mod_stmt);
}

void Admin::connect_stdgrp_to_session(Session &session){
    std::string groupID;
    while(true){
        std::cout << "Enter the Student Group ID for this session (press Enter to finish adding):";
        std::getline(std::cin, groupID);
        if(groupID.empty()){
            break;
        }
        session.addGroup(groupID);
    }
    sqlite3_stmt* stmt;
    try{
        const char* query = "INSERT INTO StdGrp_to_Session(groupID, sessionID) VALUES(?,?)";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        }else{
            std::optional<std::string> groupID;
            std::string log;
            if(groupID){std::cout << *groupID; std::cout << std::endl;}
            while(true){
                groupID = session.getGroup();
                if(!groupID){
                    std::cout << "Finished adding groups to the session" << std::endl;
                    break;
                }
                sqlite3_bind_text(stmt, 1, groupID->c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, session.getID().c_str(),-1, SQLITE_TRANSIENT);
                if(sqlite3_step(stmt)!=SQLITE_DONE){
                    std::cout << "Failed to add Group " << *groupID << " to Session " << session.getID() << std::endl;
                    std::cout << sqlite3_errmsg(db) << std::endl;
                }else{
                    log = "Added Student Group " + *groupID + " to Session " + session.getID();
                    this->push_action(log);
                }
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

void Admin::disconnect_stdgrp_to_session(Session &session)
{
    std::string groupID;
    while(true){
        std::cout << "Enter the Student Group ID to remove from this session (press Enter to finish adding):";
        std::getline(std::cin, groupID);
        if(groupID.empty()){
            break;
        }
        session.addGroup(groupID);
    }
    sqlite3_stmt* stmt;
    try{
        const char* query = "DELETE FROM StdGrp_to_Session WHERE groupID = ? AND sessionID = ?";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        }else{
            std::optional<std::string> groupID;
            std::string log;
            while(true){
                groupID = session.getGroup();
                if(!groupID){
                    std::cout << "Finished deleting groups from the session" << std::endl;
                    break;
                }
                sqlite3_bind_text(stmt, 1, groupID->c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, session.getID().c_str(),-1, SQLITE_TRANSIENT);
                if(sqlite3_step(stmt)!=SQLITE_DONE){
                    std::cout << "Failed to delete Group " << *groupID << " from Session " << session.getID() << std::endl;
                    std::cout << sqlite3_errmsg(db) << std::endl;
                }else{
                    log = "Delete Student Group " + *groupID + " from Session " + session.getID();
                    this->push_action(log);
                }
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

bool Admin::add_module_to_db(Module& module) {
    int rc = sqlite3_open(this->getFileName().c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_stmt* stmt;
    const char* ins_que = "INSERT INTO Module(moduleID, moduleName, year) VALUES(?,?,?)";

    if (sqlite3_prepare_v2(this->getDB(), ins_que, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, module.getID().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, module.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, module.getYear());

    bool success = false;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        this->push_action("Added module: " + module.getID());
        this->connect_lec_to_module(module);
        success = true;
    } else {
        std::cerr << "Step failed: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

bool Admin::delete_module_by_id(const std::string& id) {
    std::string dbPath = this->getFileName(); 
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    sqlite3_exec(this->db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to connect to database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* del_que = "DELETE FROM Module WHERE moduleID = ?";
    bool success = false;

    if (sqlite3_prepare_v2(this->db, del_que, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement\n" << sqlite3_errmsg(db) << std::endl;
    } else {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            std::cout << "Successfully deleted Module " << id << std::endl;
            this->push_action("Deleted Module: " + id);
            success = true;
        } else {
            std::cerr << "Failed to delete Module " << id << "\n" << sqlite3_errmsg(db) << std::endl;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

bool Admin::modify_module_by_id(Module& module) {
    sqlite3_stmt* select_stmt;
    const char* sel_que = "SELECT * FROM Module WHERE moduleID = ?";
    if (sqlite3_prepare_v2(this->db, sel_que, -1, &select_stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(select_stmt, 1, module.getID().c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(select_stmt) != SQLITE_ROW) {
        std::cerr << "Module ID " << module.getID() << " not found.\n";
        sqlite3_finalize(select_stmt);
        return false;
    }
    sqlite3_finalize(select_stmt);

    sqlite3_stmt* update_stmt;
    const char* upd_que = "UPDATE Module SET moduleName = ?, year = ? WHERE moduleID = ?";
    if (sqlite3_prepare_v2(this->db, upd_que, -1, &update_stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(update_stmt, 1, module.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(update_stmt, 2, module.getYear());
    sqlite3_bind_text(update_stmt, 3, module.getID().c_str(), -1, SQLITE_TRANSIENT);

    bool success = false;
    if (sqlite3_step(update_stmt) == SQLITE_DONE) {
        std::cout << "Module " << module.getID() << " successfully updated.\n";
        this->push_action("Modified Module: " + module.getID());
        success = true;
    } else {
        std::cerr << "Failed to update Module: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(update_stmt);
    std::string choiceInput;
    int choice = 0;

    std::cout << "Enter an option (1 for adding lecturer, 2 for removing, press Enter for none): ";
    std::getline(std::cin, choiceInput);

    if (!choiceInput.empty()) {
        try {
            choice = std::stoi(choiceInput);
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid input. Please enter 1, 2, or press Enter." << std::endl;
        }
    }

    switch (choice) {
        case 1:
            this->connect_lec_to_module(module);
            break;
        case 2:
            this->disconnect_lec_to_module(module);
            break;
        default:
            break;
    }
    sqlite3_close(db);
    return success;
}

bool Admin::add_stdgrp_to_db(StudentGroup& group){
    sqlite3_stmt* stdgrp_stmt;
    try{
        const char* stdgrp_que = "INSERT INTO StudentGroup(groupID) VALUES(?)";
        if(sqlite3_prepare_v2(this->db, stdgrp_que, -1, &stdgrp_stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement\n" << sqlite3_errmsg(db) << std::endl;
            return false;
        }else{
            sqlite3_bind_text(stdgrp_stmt, 1, group.getGroupID().c_str(), -1, SQLITE_TRANSIENT);
            if(sqlite3_step(stdgrp_stmt)!=SQLITE_DONE){
                std::cout << "Failed to add student group " << group.getGroupID() << std::endl << sqlite3_errmsg(db) << std::endl; 
                return false;
            }else{
                std::cout << "Successfully added student group: " << group.getGroupID() << std::endl;
                std::string log = "Added student group: " + group.getGroupID();
                this->push_action(log);
                this->connect_stdgrp_to_course(group);
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    sqlite3_finalize(stdgrp_stmt);
    return true;
}

bool Admin::delete_stdgrp_from_db(StudentGroup& group){
    sqlite3_stmt* stmt;
        try{
            const char* del_que = "DELETE FROM StudentGroup WHERE groupID = ?";
            if(sqlite3_prepare_v2(this->db, del_que, -1, &stmt, nullptr)!=SQLITE_OK){
                std::cout << "Failed to prepare statement\n" << sqlite3_errmsg(db) << std::endl;
                return false;
            }else{
                sqlite3_bind_text(stmt, 1, group.getGroupID().c_str(), -1, SQLITE_TRANSIENT);
                if(sqlite3_step(stmt)!=SQLITE_DONE){
                    std::cout << "Failed to delete Group " << group.getGroupID() << "\n" << sqlite3_errmsg(db) << std::endl;
                    return false;
                }else{
                    std::cout << "Successfully deleted Student Group " << group.getGroupID()<< std::endl;
                    std::string log = "Deleted Student Group: " + group.getGroupID();
                    this->push_action(log);
                }
            }
        }catch(std::exception& e){
            std::cout << e.what() << std::endl;
            return false;
        }
        sqlite3_finalize(stmt);
        return true;
}

bool Admin::add_session_to_db(Session& session){
    sqlite3_stmt* stmt;
    try{
        const char* insert = "INSERT INTO Session(sessionID, sessionName, moduleID, scheduledAt, weekNo, roomID, duration) VALUES(?,?,?,?,?,?,?)";
        if(sqlite3_prepare_v2(db, insert, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db) << std::endl;
            return false;
        }else{
            sqlite3_bind_text(stmt, 1, session.getID().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, session.getName().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, session.getModule().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, session.getDateTime().datetime().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 5, session.getWeek());
            sqlite3_bind_text(stmt, 6, session.getRoom().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 7, session.getDuration());
            if(sqlite3_step(stmt)!=SQLITE_DONE){
                std::cout << "Failed to create session: " << sqlite3_errmsg(db)<<std::endl;
                sqlite3_finalize(stmt);
                return false;
            }else{
                std::cout << "Created session:" << session.getID() << std::endl;
                std::string log = "Created Session: " + session.getID();
                this->push_action(log);
                this->connect_stdgrp_to_session(session);
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool Admin::modify_session_in_db(Session& session){
    sqlite3_stmt* update_stmt;
    std::string upd_que = "UPDATE Session SET sessionName = ? , lecturerID = ?, moduleID = ?, scheduledAt = ?, roomID = ?, weekNo = ? WHERE sessionID = ?";
    if(sqlite3_prepare_v2(db, upd_que.c_str(), -1, &update_stmt, nullptr)!=SQLITE_OK){
        std::cout << "Failed to prepare statement\n" << sqlite3_errmsg(db) << std::endl;
        return false;
    }else{
        sqlite3_bind_text(update_stmt, 1, session.getName().c_str(), -1, SQLITE_TRANSIENT);
        if (session.getLecturer().empty()){
            sqlite3_bind_null(update_stmt, 2);
        } else {
            sqlite3_bind_text(update_stmt, 2, session.getLecturer().c_str(), -1, SQLITE_TRANSIENT);
        }
        sqlite3_bind_text(update_stmt, 3, session.getModule().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(update_stmt, 4, session.getDateTime().datetime().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(update_stmt, 5, session.getRoom().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(update_stmt, 6, session.getWeek());
        sqlite3_bind_text(update_stmt, 7, session.getID().c_str(), -1, SQLITE_TRANSIENT);

        if(sqlite3_step(update_stmt)==SQLITE_DONE){
            std::cout << "Successfully session " << session.getID() << std::endl;
            std::string log = "Updated Session: " + session.getID() + ", " + session.getName() + ", at " + session.getRoom()
                + ", on " + session.getDateTime().datetime() + ", for " + session.getModule() + ", taught by " + session.getLecturer();
            this->push_action(log);

            int choice;
            std::cout << "Enter 1 if you want to add a student group, or 2 if you want to remove a student group,\nAnd Press Enter if neither:";
            std::cin>>choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch(choice){
                case 1: {
                    this->connect_stdgrp_to_session(session);
                    break;
                }
                case 2: {
                    this->disconnect_stdgrp_to_session(session);
                    break;
                }
                default:{
                    break;
                }
            }
        }else{
            std::cout << "Failed to update session\n" << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(update_stmt);
            return false;
        }
    }
    sqlite3_finalize(update_stmt);
    return true;
}

bool Admin::add_student_to_db(Student& student){
    sqlite3_stmt* stmt;
    try{
        const char* query = "INSERT INTO Student(username, password, name, course) VALUES(?,?,?,?)";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }else{
            sqlite3_bind_text(stmt, 1, student.getUsername().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, student.getPassword().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, student.getName().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, student.getCourse().c_str(), -1, SQLITE_TRANSIENT);

            if(sqlite3_step(stmt)!=SQLITE_DONE){
                std::cout << "Failed to register Student: " << student.getUsername() << "\n" << sqlite3_errmsg(db) << std::endl;
                return false;
            }else{
                std::cout << "Registered Student " << student.getUsername() << " successfully" <<std::endl;
                std::string log = "Registered Student " + student.getUsername();
                this->push_action(log);
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool Admin::add_student_to_std_grp(Student& student){
    sqlite3_stmt* stmt;
    try{
        const char* query = "INSERT INTO Std_to_StdGrp(studentID, groupID) VALUES(?,?)";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db) << std::endl;
            return false;
        }else{
            while(true){
                std::optional<std::string> groupID = student.getGroupID();
                if(!groupID){
                    std::cout << "Finished assigning student to groups" << std::endl;
                    break;
                }
                sqlite3_bind_text(stmt, 1, student.getUsername().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, groupID->c_str(), -1, SQLITE_TRANSIENT);
                if(sqlite3_step(stmt)!=SQLITE_DONE){std::cout << "Failed to assign student to group " << *groupID << "\n"<<sqlite3_errmsg(db)<<std::endl;}
                else{
                    this->push_action("Assigned Student " + student.getUsername() + " to Student Group " + *groupID);
                }
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
            }
        }
        sqlite3_finalize(stmt);
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Admin::add_lecturer_to_db(Tutor& tutor){
    sqlite3_stmt* stmt;
    try{
        const char* query = "INSERT INTO Lecturer(username, password, name) VALUES(?,?,?)";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }else{
            sqlite3_bind_text(stmt, 1, tutor.getUsername().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, tutor.getPassword().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, tutor.getName().c_str(), -1, SQLITE_TRANSIENT);

            if(sqlite3_step(stmt)!=SQLITE_DONE){
                std::cout << "Failed to register Lecturer " << tutor.getUsername() << " " << tutor.getName() << std::endl;
                std::cout << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return false;
            }else{
                std::cout << "Successfully registered Lecturer: " << tutor.getUsername() << std::endl;
                this->push_action("Registered Lectuter: "+ tutor.getUsername());
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Admin::check_session_and_lec(const std::string& sessionID, const std::string& lecturerID){
    sqlite3_stmt* stmt;
    try{
        const char* query = "SELECT EXISTS(SELECT 1 FROM Session WHERE SessionID = ?) AS SessionExists,EXISTS(SELECT 1 FROM Lecturer WHERE Username = ?) AS LecturerExists";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }else{
            sqlite3_bind_text(stmt, 1, sessionID.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, lecturerID.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                bool sessionExists = sqlite3_column_int(stmt, 0);
                bool lecturerExists = sqlite3_column_int(stmt, 1);
                sqlite3_finalize(stmt);
                if(sessionExists && lecturerExists){return true;}
                if(!sessionExists){
                    std::cout << "No session found" << std::endl;
                }
                if(!lecturerExists){
                    std::cout << "No Lecturer found" << std::endl;
                }
                return false;
            }
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    return false;
}

bool Admin::connect_lec_to_session(const std::string& sessionID, const std::string& lecturerID){
    sqlite3_stmt* stmt;
    try{
        const char* query = "UPDATE Session SET lecturerID = ? WHERE sessionID = ?";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) <<std::endl;
            return false;
        }else{
            sqlite3_bind_text(stmt, 1, lecturerID.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, sessionID.c_str(), -1, SQLITE_TRANSIENT);
            if(sqlite3_step(stmt)!=SQLITE_DONE){
                std::cout << "Failed to assign Lecturer" << lecturerID << " to Session " << sessionID << "\n" << sqlite3_errmsg(db) << std::endl;
                return false;
            }else{
                std::cout << "Successfully assigned Lecturer " << lecturerID << " to Session " << sessionID << std::endl;
                this->push_action("Assigned Lecturer: " + lecturerID + " to Session: " + sessionID);
            }
        }
        sqlite3_finalize(stmt);
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Admin::create_timetable(Timetable& tt){
    sqlite3_stmt* check_stmt;
    std::cout << "Working till here" << std::endl;

    try {
        const char* check_query = "SELECT weekNo, startDate, endDate FROM Timetable WHERE weekNo = ?";
        std::cout << "Preparing SQL query..." << std::endl;

        // Prepare the SQL statement
        if (sqlite3_prepare_v2(db, check_query, -1, &check_stmt, nullptr) != SQLITE_OK) {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // Bind week number to the query
        sqlite3_bind_int(check_stmt, 1, tt.getWeek());

        // Step through the statement and check if there's a result
        int rc = sqlite3_step(check_stmt);
        if (rc != SQLITE_ROW) {
            std::cout << "No week found for this year" << std::endl;
            sqlite3_finalize(check_stmt);
            return false;
        }

        // Extract the start and end date from the result
        const char* start_date_raw = reinterpret_cast<const char*>(sqlite3_column_text(check_stmt, 1));
        const char* end_date_raw = reinterpret_cast<const char*>(sqlite3_column_text(check_stmt, 2));

        // Handle potential NULL values in the result
        if (!start_date_raw || !end_date_raw) {
            std::cout << "Error: Missing date values for the timetable" << std::endl;
            sqlite3_finalize(check_stmt);
            return false;
        }

        std::string start_date(start_date_raw);
        std::string end_date(end_date_raw);

        // Set the start and end times
        tt.setStart(DateTime(start_date, "00:00:00"));
        tt.setEnd(DateTime(end_date, "23:59:59"));

        // Create dummy session for start and end times to use in the loop
        Session dummy_start = Session("Dummy_Start", start_date, "00:00:00", "dummy_room");
        Session dummy_end = Session("Dummy_End", end_date, "23:59:59", "dummy_room");

        int choice;
        bool adding = true;
        while (adding) {
            std::cout << "Press 1 if you want to add session and press 2 if you want to finish adding: ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Week No.: " << tt.getWeek() << std::endl;
            std::cout << start_date << " to " << end_date << std::endl;

            switch (choice) {
                case 1: {
                    // Create session
                    Session session = this->createSession();

                    // Check if the session falls within the valid week range
                    if (session < dummy_start || dummy_end < session) {
                        std::cout << "Session not in the week!! Please enter the dates in the time period." << std::endl;
                    } else {
                        // Add the session to the database
                        this->add_session_to_db(session);
                    }
                    break;
                }
                case 2: {
                    // Exit adding loop
                    adding = false;
                    break;
                }
                default: {
                    // Exit loop on any invalid choice
                    adding = false;
                    break;
                }
            }
        }

        // Finalize the prepared statement after use
        sqlite3_finalize(check_stmt);
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        sqlite3_finalize(check_stmt);
        return false;
    }

    return true;
}

const Session Admin::createSession(){
    std::string SessionID;
    std::string GroupID;
    std::string SessionName;
    std::string ModuleID;
    std::string date;
    std::string time;
    std::string RoomID;
    float duration;

    std::cout << "Enter Session ID:" ;
    std::getline(std::cin, SessionID);

    std::cout << "Enter name for session:";
    std::getline(std::cin, SessionName);

    std::cout << "Enter Student Group ID:";
    std::getline(std::cin, GroupID);
    
    std::cout << "Enter Module ID:";
    std::getline(std::cin, ModuleID);

    std::cout << "Enter Room ID:";
    std::getline(std::cin, RoomID);

    std::cout << "Enter Date (dd-mm-yyyy):";
    std::getline(std::cin, date);
    std::cout << "Enter time (hh:mm:ss)(24h format):";
    std::getline(std::cin, time);

    std::cout << "Enter duration (in hours [float]):";
    std::cin >> duration;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Session session = Session(SessionID, date, time, RoomID);
    session.setModule(ModuleID);
    session.setName(SessionName);
    session.setDuration(duration);
    session.addGroup(GroupID);

    return session;
}

Timetable* Admin::retrieveTimetable(const int& week){
    Timetable* timetable = new Timetable(week);
    sqlite3_stmt* tt_fetch_stmt;
    try{
        const char* tt_fetch_query = "SELECT * FROM Timetable WHERE weekNo = ?";
        if(sqlite3_prepare_v2(db, tt_fetch_query, -1, &tt_fetch_stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            delete timetable;
            return nullptr;
        }else{
            sqlite3_bind_int(tt_fetch_stmt, 1, timetable->getWeek());
            if(sqlite3_step(tt_fetch_stmt)!=SQLITE_ROW){
                std::cout << "No timetable found for the week" << std::endl;
                sqlite3_finalize(tt_fetch_stmt);
                return nullptr;
            }else{
                std::string start_date = reinterpret_cast<const char*>(sqlite3_column_text(tt_fetch_stmt, 1));
                std::string end_date = reinterpret_cast<const char*>(sqlite3_column_text(tt_fetch_stmt, 2));

                timetable->setStart(DateTime(start_date, "00:00:00"));
                timetable->setEnd(DateTime(end_date, "23:59:59"));
                this->fetchTimetable(timetable);
            }
            sqlite3_finalize(tt_fetch_stmt);
        }
    }catch(std::exception& e){
        delete timetable;
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return timetable;
}

void Admin::fetchTimetable(Timetable* tt){
    sqlite3_stmt* stmt;
    try{
        const char* query = "SELECT * FROM Session WHERE weekNo = ?";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl; 
        }else{
            std::cout << tt->getWeek() << std::endl;
            sqlite3_bind_int(stmt, 1, tt->getWeek());
            while(sqlite3_step(stmt)==SQLITE_ROW){
                std::string sessionID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                std::string sessionName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                std::string moduleID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                std::string lecturerID = (sqlite3_column_type(stmt, 3) == SQLITE_NULL) ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                std::string scheduledAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                int weekNo = sqlite3_column_int(stmt, 5);
                std::string roomID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                double duration = sqlite3_column_double(stmt, 7);
                std::cout << duration << std::endl;
                std::vector<std::string> sch_time = split(scheduledAt, ' ');
                if (sch_time.size() < 2) {
                    std::cout << "Invalid scheduledAt format.\n";
                    continue;
                }else{
                    Session session = Session(sessionID, sch_time[0], sch_time[1], roomID);
                    session.setName(sessionName);
                    session.setLecturer(lecturerID);
                    session.setModule(moduleID);
                    session.addGroup(this->fetchGroups(session.getID()));
                    session.setDuration(duration);
                    tt->addSession(session);
                    
                }
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

std::vector<std::string> Admin::fetchGroups(const std::string& sessionID){
    sqlite3_stmt* stmt;
    std::vector<std::string> groups;
    try{
        const char* query = "SELECT groupID FROM StdGrp_to_Session WHERE sessionID = ?";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return std::vector<std::string>();
        }else{
            sqlite3_bind_text(stmt, 1, sessionID.c_str(), -1, SQLITE_TRANSIENT);
            while(sqlite3_step(stmt)==SQLITE_ROW){
                std::string groupID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                groups.push_back(groupID);
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return std::vector<std::string>();
    }
    return groups;
}

void Admin::delete_session(const std::string& sessionID){
    sqlite3_stmt* stmt;
    try{
        const char* query = "DELETE FROM Session WHERE sessionID=?";
        if(sqlite3_prepare_v2(db,query, -1, &stmt, nullptr)!=SQLITE_OK){
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        }else{
            sqlite3_bind_text(stmt, 1, sessionID.c_str(), -1, SQLITE_TRANSIENT);
            if(sqlite3_step(stmt)==SQLITE_DONE){
                std::cout << "Successfully deleted session" << std::endl;
                this->push_action("Deleted session: "+sessionID);
            }else{
                std::cout << "Failed to delete session" << std::endl;
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

bool Admin::groupConflict(){
    sqlite3_stmt* stmt;
    try{
        const char* query = R"(
            SELECT 
                sg1.studentID,
                s1.sessionID AS session1,
                s2.sessionID AS session2,
                s1.scheduledAT AS scheduledAT1,
                s1.duration AS duration1,
                s2.scheduledAT AS scheduledAT2,
                s2.duration AS duration2,
                s1.weekNo
            FROM 
                Std_to_StdGrp sg1
            JOIN 
                Std_to_StdGrp sg2 ON sg1.studentID = sg2.studentID AND sg1.groupID != sg2.groupID
            JOIN 
                StdGrp_to_Session g1 ON sg1.groupID = g1.groupID
            JOIN 
                StdGrp_to_Session g2 ON sg2.groupID = g2.groupID AND g1.sessionID != g2.sessionID
            JOIN 
                Session s1 ON g1.sessionID = s1.sessionID
            JOIN 
                Session s2 ON g2.sessionID = s2.sessionID
            WHERE 
                s1.weekNo = s2.weekNo
                AND (
                    (strftime('%s', s1.scheduledAT) + (s1.duration * 3600)) > strftime('%s', s2.scheduledAT) 
                    AND strftime('%s', s1.scheduledAT) < (strftime('%s', s2.scheduledAT) + (s2.duration * 3600))
                );
        )";
        if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr)!=SQLITE_OK){
            return false;
        }else{
            while(sqlite3_step(stmt)==SQLITE_ROW){
                std::string studentID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                std::string session1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                std::string session2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                std::cout << "Student Conflict" << std::endl << "Student ID: " << studentID << std::endl << "Sessions: " << session1 << ", " << session2 << std::endl;
            }
            sqlite3_finalize(stmt);
        }
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Admin::roomConflict(){
    sqlite3_stmt* stmt;

    try {
        const char* query = R"(
            SELECT 
                s1.roomID,
                s1.sessionID AS session1,
                s2.sessionID AS session2,
                s1.scheduledAT AS scheduledAT1,
                s1.duration AS duration1,
                s2.scheduledAT AS scheduledAT2,
                s2.duration AS duration2,
                s1.weekNo
            FROM 
                Session s1
            JOIN 
                Session s2 ON s1.roomID = s2.roomID AND s1.sessionID != s2.sessionID
            WHERE 
                s1.weekNo = s2.weekNo
                AND (
                    (strftime('%s', s1.scheduledAT) + (s1.duration * 3600)) > strftime('%s', s2.scheduledAT) 
                    AND strftime('%s', s1.scheduledAT) < (strftime('%s', s2.scheduledAT) + (s2.duration * 3600))
                );
        )";

        if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string roomID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string session1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string session2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::cout << "Room Conflict" << std::endl << "Room ID: " << roomID << std::endl << "Sessions: " << session1 << ", " << session2 << std::endl;
            
        }

        sqlite3_finalize(stmt);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Admin::lecturerConflict(){
    sqlite3_stmt* stmt;

    try {
        const char* query = R"(
            SELECT 
                s1.lecturerID,
                s1.sessionID AS session1,
                s2.sessionID AS session2,
                s1.scheduledAT AS scheduledAT1,
                s1.duration AS duration1,
                s2.scheduledAT AS scheduledAT2,
                s2.duration AS duration2,
                s1.weekNo
            FROM 
                Session s1
            JOIN 
                Session s2 ON s1.lecturerID = s2.lecturerID AND s1.sessionID != s2.sessionID
            WHERE 
                s1.weekNo = s2.weekNo
                AND (
                    (strftime('%s', s1.scheduledAT) + (s1.duration * 3600)) > strftime('%s', s2.scheduledAT) 
                    AND strftime('%s', s1.scheduledAT) < (strftime('%s', s2.scheduledAT) + (s2.duration * 3600))
                );
        )";

        if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string lecturerID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string session1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string session2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::cout << "Lecturer Conflict" << std::endl << "Lecturer ID: " << lecturerID << std::endl << "Sessions: " << session1 << ", " << session2 << std::endl;
        }
        sqlite3_finalize(stmt);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}
    

bool Admin::studentConflict(){
    sqlite3_stmt* stmt;
    try {
        const char* query = R"(
            SELECT 
                sg1.studentID,
                s1.sessionID AS session1,
                s2.sessionID AS session2,
                s1.scheduledAT AS scheduledAT1,
                s1.duration AS duration1,
                s2.scheduledAT AS scheduledAT2,
                s2.duration AS duration2,
                s1.weekNo
            FROM 
                Std_to_StdGrp sg1
            JOIN 
                Std_to_StdGrp sg2 ON sg1.studentID = sg2.studentID AND sg1.groupID != sg2.groupID
            JOIN 
                StdGrp_to_Session g1 ON sg1.groupID = g1.groupID
            JOIN 
                StdGrp_to_Session g2 ON sg2.groupID = g2.groupID AND g1.sessionID != g2.sessionID
            JOIN 
                Session s1 ON g1.sessionID = s1.sessionID
            JOIN 
                Session s2 ON g2.sessionID = s2.sessionID
            WHERE 
                s1.weekNo = s2.weekNo
                AND (
                    (strftime('%s', s1.scheduledAT) + (s1.duration * 3600)) > strftime('%s', s2.scheduledAT) 
                    AND strftime('%s', s1.scheduledAT) < (strftime('%s', s2.scheduledAT) + (s2.duration * 3600))
                );
        )";

        if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string studentID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string session1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string session2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::cout << "Student Conflict" << std::endl << "Student ID: " << studentID << std::endl << "Sessions: " << session1 << ", " << session2 << std::endl;
        }

        sqlite3_finalize(stmt);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}
