# PlaTwo

PlaTwo is desktop multiplayer board game platform developed with Qt and C++. 
The project provides a unified environment for playing multiple classic board games with player accounts, game history , and save / load functionality.

## Overview

PlaTwo is designed to manage multiple local multiplayer games through a single application. Each player can create an account, edit their profile, save unfinished games, and continue them later.

## Games
- Dots and Boxes
- Fanorona
- Nine Men's Morris

## Features 
### Player Management
- Player registration and login
- Editable player profiles
- Personal statistics
- Secure password storage

### Gameplay
- Local multiplayer support
- Turn management
- Game validation
- winner detection
- Auto start of games

### Save & Load
- Save games at low intervals
- Load previously saved games
- Resume unfinished matches

### Game History
- Store completed matches
- View previous games
- Display game information and results

### User Interface
- Modern Qt Widgets interface
- Separate windows for each section
- Responsive navigation between pages

## Technologies 

- C++
- Qt Widgets
- Qt Core
- Qt GUI
- JSON
- qmake

## Project Structure

```
PlaTwo/
│
├── auth/
├── core/
├── games/
│   ├── dotsandboxes/
│   ├── fanorona/
│   └── morris/
├── network/
├── utils/
├── ui/
    ├── forms/
├── resources/
└── main.cpp
```
## Requirements

- Qt 6.x
- C++17 or newer
- MinGW or MSVC

## Build

Open the `.pro` file using Qt Creator.

Then:

1. Configure the project.
2. Build.
3. Run.

Or from the command line:

```bash
qmake
mingw32-make
```

## Authors

Developed as a university project using Qt and C++ by Mohamadreza Ahmadi and Seyed Erfan Sorouri.

## License

This project is intended for educational purposes.
