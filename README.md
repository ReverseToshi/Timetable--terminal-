# Timetable (terminal)
## University Project

A console-based timetabling system designed for Nottingham Trent University (NTU) to manage and view academic schedules efficiently.

## 📖 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Development](#development)
- [Contributing](#contributing)
- [License](#license)

## ✨ Overview

This application provides a simplified, role-based timetable management system that enables:
- **Students** to view and search their schedules
- **Lecturers** to access teaching timetables and student schedules
- **Administrators** to manage modules, groups, sessions, and generate timetables

The system features conflict detection, cross-platform compatibility, and uses SQLite for data persistence.

## 🚀 Features

### 👨‍🎓 For Students
- View current week's timetable
- Search timetable by week number, room, module, or lecturer
- Display academic weeks with date ranges
- Export timetable to Excel format

### 👨‍🏫 For Lecturers
- View personal teaching schedule
- Search student timetables
- Filter by week, room, or module
- Export functionality

### 👨‍💼 For Administrators
- Manage modules (add/delete/modify)
- Handle student groups and registrations
- Define and modify sessions
- Assign lecturers to sessions
- Generate and update weekly timetables
- Detect scheduling conflicts

## 🛠️ Technology Stack

- **Programming Language**: C++
- **Database**: SQLite
- **Libraries**: sqlite3.h, Standard Template Library (STL)
- **Compatibility**: Windows, macOS, Linux

## 📦 Installation

### Prerequisites
- C++ compiler (g++ recommended)
- SQLite development libraries

### Installation Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/your-username/ntu-timetable-app.git
   cd ntu-timetable-app```

2. **Compile the application**
    ```bash
    cmake -S ./src -B .
    ```

3. **Run the application**
    ```bash
    cd ./build
    ./NTU_Timetable
    ```
