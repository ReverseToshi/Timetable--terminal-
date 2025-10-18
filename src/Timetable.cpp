#include <Timetable.hpp>
#include <fstream>

static std::string escapeCSV(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else          out += c;
    }
    out += '"';
    return out;
}

Timetable::Timetable(const int& week){
    this->weekNo = week;
}

const int Timetable::getWeek(){
    return this->weekNo;
}

const DateTime Timetable::getStart()
{
    return this->Start;
}

const DateTime Timetable::getEnd()
{
    return this->End;
}

void Timetable::addSession(Session& session)
{
    this->Sessions.insert(session);
}

void Timetable::setStart(const DateTime &start)
{
    this->Start = start;
    this->Start.calculateWeek();
}

void Timetable::setEnd(const DateTime &end)
{
    this->End =  end;
    this->End.calculateWeek();
}

std::optional<Session> Timetable::getSession()
{
    if(this->Sessions.empty()){return std::nullopt;}
    auto It = *this->Sessions.begin();
    this->Sessions.erase(this->Sessions.begin());
    return It;
}

void Timetable::printTimetable(){
    std::cout << "Timetable for Week " << weekNo << std::endl;
    std::cout << "Start Date: " << this->Start.datetime() << std::endl;
    std::cout << "End Date: " << this->End.datetime() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    std::string currentDay = "";
    while(true){
        std::optional<Session> session = this->getSession();
        if(!session){
            break;
        }
        std::string sessionDay = session->getDateTime().getDayOfTheWeek();

        // Print the day header if it's a new day
        if (sessionDay != currentDay) {
            currentDay = sessionDay;
            std::cout << "\n" << currentDay << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << std::left << std::setw(10) << "ID"
                      << std::setw(20) << "Name"
                      << std::setw(20) << "Day"
                      << std::setw(20) << "DateTime"
                      << std::setw(10) << "Room"
                      << std::setw(10) << "Lecturer"
                      << std::setw(10) << "Duration" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }

        std::cout << std::left << std::setw(10) << session->getID()
                  << std::setw(20) << session->getName()
                  << std::setw(20) << session->getDateTime().getDayOfTheWeek()
                  << std::setw(20) << session->getDateTime().datetime()
                  << std::setw(10) << session->getRoom()
                  << std::setw(10) << session->getLecturer()
                  << std::setw(10) << session->getDuration() << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;
}

void Timetable::exportCSV(const std::string& filename) {
    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV file for writing: " << filename << "\n";
        return;
    }

    // 1) write header
    file 
      << "Week"
      << ",Date"
      << ",Time"
      << ",Day"
      << ",SessionID"
      << ",Name"
      << ",Module"
      << ",Lecturer"
      << ",Room"
      << ",Groups"
      << ",Duration"
      << "\n";

    // 2) write each session
    while(true) {
        std::optional<Session> sess = this->getSession();
        if(!sess){break;}
        DateTime dt = sess->getDateTime();
        // join groups if your Session has multiple—here we only use getGroup()
        std::string grpList;
        for (auto const& g : sess->getGroups()) {
            grpList += g;
            grpList += ';';
        }
        // if you have a getGroups() vector, you could do:
        //   for (auto &g2 : sess.getGroups()) { groups += g2 + ";"; }
        //   if (!groups.empty()) groups.pop_back();

        file
          // week number
          << sess->getWeek() << ','

          // date, time, day
          << escapeCSV(dt.getDate()) << ','
          << escapeCSV(dt.getTime()) << ','
          << escapeCSV(dt.getDayOfTheWeek()) << ','

          // session fields
          << escapeCSV(sess->getID())      << ','
          << escapeCSV(sess->getName())    << ','
          << escapeCSV(sess->getModule())  << ','
          << escapeCSV(sess->getLecturer())<< ','
          << escapeCSV(sess->getRoom())    << ','
          << escapeCSV(grpList)            << ','
          << sess->getDuration()
          << "\n";
    }

    file.close();
}

