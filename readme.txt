==========================================================================================================
Building AlbumEasy on Linux or Windows
==========================================================================================================


1. Prerequisites:
----------------------------------------------------------------------------------------------------------
AlbumEasy is dependant on the Qt application framework available from the
Qt Project http://qt-project.org/
The current version of AlbumEasy has been built with Qt 5.1


2. Windows only prerequisites:
----------------------------------------------------------------------------------------------------------
For Windows be sure to download and install the MinGW version of Qt as this also
includes the necessary MinGW build tools.
e.g qt-windows-opensource-5.1.0-mingw48_opengl-x86-offline

Windows also requires the bash shell which is part of Cygwin http://www.cygwin.com

Inno Setup from http://www.jrsoftware.org/isinfo.php is required for building the installation.
Select the Unicode version and make sure to include the "Inno Setup Preprocessor".

It is also necessary to point the Cygwin environment to the MinGW build tools, this can
be accomplished by editing the .bashrc in the user's home directory and adding the
following line to the end:

export PATH=/cygdrive/c/Qt/Qt5.1.0/5.1.0/mingw48_32/bin/:/cygdrive/c/Qt/Qt5.1.0/Tools/mingw48_32/bin/:$PATH


3. Build configuration:
----------------------------------------------------------------------------------------------------------
Prior to building, edit the build script called "Makes" modifying the following environment
variables to suit the local environment:
WIN_QT_LOCATION
WIN_INNO_SETUP


4. Building
----------------------------------------------------------------------------------------------------------
In a Linux shell or at the Cygwin prompt change to the directory containing the source files
then execute the build script:
./Makes

The build script takes optional parameters as follows:
./Makes               Release version
./Makes RELEASE        "       "
./Makes SETUP         Release version and packaged setup
./Makes DEBUG         Debug version
./Makes CLEAN         Remove generated files and intermediate build files
./Makes DISTCLEAN     Also removes the application and all other generated files e.g. setup etc.
