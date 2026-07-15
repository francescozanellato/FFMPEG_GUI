# FFMPEG_GUI
FFMPEG_GUI - A GUI for FFMPEG and any commandline tool (QPDF, PDFTK, PDFUnisci, etc.)

    Author: Francesco Paolo Luca Zanellato
    Copyright (C) 2026 Francesco Paolo Luca Zanellato

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

# Features

Select a shortcut from the Shortcuts drop down list and run the command:

<img width="1135" height="733" alt="image018" src="https://github.com/user-attachments/assets/a64e71f1-a508-4eb0-8818-74c05b09845f" />




You can use this GUI with ANY COMMANDLINE PROGRAM (e.g. qpdf, pdftk, pdfUnisci, etc.) on Windows or other systems supported by Qt (linux, macOS, etc.): just rename the .exe and .ini (and .vbs) files (e.g. QPDF_GUI.exe, QPDF_GUI.ini, QPDF_GUI.vbs), tweak the .ini file to fit your custom executable and the relevant [Combinations] section in the .ini file. Run the app and use your custom combinations with your commandline executable.




The program supports up to 5 commandline input files (or just drag and drop up to 5 files on the executable or .vbs script).





The project is primarily written in C++ and utilizes QMake for building and project management. Its user interface and functionalities are designed to speed up the FFMPEG commands mostly used.

The commands can be customized.

Example Usage (from commandline/bash environment):

    ./FFMPEG_GUI

By modifying the FFMPEG_GUI.ini file (located in the same folder of the executable file), you can set up your own code snippets and configure other options to add other commands for FFMPEG.

# Configuration

FFMPEG_GUI allows you to configure various options through the `FFMPEG_GUI.ini` configuration file. This file lets you customize different settings to tailor the application's behavior to your needs.

Example Configuration

```ini
[Combinations]
MuxVideoWithAudio1=-i %1 -i %2 -c:v copy -c:a aac -map 0:v:0 -map 1:a:0 %5
ExtractAudioFromVideo=-i %1 -vn -acodec libmp3lame %5
```


# Compiling (recommended method on Windows):

Install MSYS2, following the instructions at the following link:

https://www.msys2.org/wiki/MSYS2-installation/

Run MSYS2 (Mingw64 environment):

    C:\msys64\mingw64.exe

Install Qt package and all the development tools with the following commands inside MSYS2 (Mingw64 environment):

    pacman -S mingw-w64-x86_64-qt6
    pacman -S mingw-w64-x86_64-make
    pacman -S git
    pacman -S mingw-w64-x86_64-gcc

Ensure that you have access to C:\ and that you can write directories in the C:\ location.
Also ensure that no files are present in "C:\PortableApps\FFMPEG_GUI\" (or that this folder does not exist), otherwise, make a backup of these files, since the below commands will overwrite the contents.

Clone the Repository, build the project, run and deploy the application:

    git clone https://github.com/francescozanellato/FFMPEG_GUI.git
    cd FFMPEG_GUI
    qmake6
    mingw32-make.exe
	export MY_PROJECT_DEPLOYMENT_PATH="/c/PortableApps/FFMPEG_GUI"
	export PATH=./bin:./:$PATH
    mkdir -p $MY_PROJECT_DEPLOYMENT_PATH
    cp ./release/FFMPEG_GUI.exe $MY_PROJECT_DEPLOYMENT_PATH
    cp ./release/FFMPEG_GUI.vbs $MY_PROJECT_DEPLOYMENT_PATH
    cd $MY_PROJECT_DEPLOYMENT_PATH
    windeployqt6 --plugindir ./share/qt6/plugins $MY_PROJECT_DEPLOYMENT_PATH/FFMPEG_GUI.exe
    find $MY_PROJECT_DEPLOYMENT_PATH/share/ -type f -executable | xargs ldd | grep /mingw64 | awk '{print $3}' | xargs -i cp {} $MY_PROJECT_DEPLOYMENT_PATH/
    find $MY_PROJECT_DEPLOYMENT_PATH/ -type f -executable | xargs ldd | grep /mingw64 | awk '{print $3}' | xargs -i cp {} $MY_PROJECT_DEPLOYMENT_PATH/
    cp /mingw64/bin/libgif-7.dll ./
    rm -f D3Dcompiler_47.dll
    explorer.exe /select,"C:\\PortableApps\\FFMPEG_GUI\\FFMPEG_GUI.exe"
    $MY_PROJECT_DEPLOYMENT_PATH/FFMPEG_GUI.exe &

In Windows, create the program shortcut on the Desktop, pointing to the following path:

    "C:\PortableApps\FFMPEG_GUI\FFMPEG_GUI.exe"

Double click the link to open the program.

If you need to move the program folder in a different location, double click on the script "FFMPEG_GUI.vbs" instead of on "FFMPEG_GUI.exe". This vbs script sets some "path" variables before running the application. Therefore, after moving the program folder (e.g. to C:\PortableApps_new), you can create a shortcut on the Desktop pointing to the following path (where "C:\PortableApps_new" is the new location):

    "C:\PortableApps_new\FFMPEG_GUI\FFMPEG_GUI.vbs"

Note: after compiling, the folder "C:\msys64\home\fraz3\FFMPEG_GUI" can be removed. You can leave the folder "C:\msys64" for compiling other programs.


# Compiling (alternative method for all operating systems):


# Compiling

Install Qt and QMake:

    Ensure that you have Qt and QMake installed on your system. You can download and install Qt from the Qt official website.

Clone the Repository:

    git clone https://github.com/francescozanellato/FFMPEG_GUI.git
    cd FFMPEG_GUI

Generate Makefile using QMake:

    qmake

Compile the Project:

    make

Run the Application:

    ./FFMPEG_GUI



# Credits

Thanks to https://gemini.google.com for the AI support on coding.
