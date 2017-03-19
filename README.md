# group-project
Cranfield group project

To run the project open Visual Studio and install the 3 Qt extensions.

After that restart VS, click the Qt5 menu then Qt Options and add your current Qt version. It should look something like this: C:\Qt\Qt5.6.2\5.6\msvc2015. If you have the 64-bit version (the name should be msvc2015-x64 or something) then it probably won't work, you need to download the 32-bit Qt (http://download.qt.io/official_releases/qt/5.6/5.6.2/qt-opensource-windows-x86-msvc2015-5.6.2.exe).

The CUDA toolkit v8.0 must also be installed.

If you want to run the generated EXE files on your computer without using the configured Visual Studio, you will need to add the Qt SDK location to your PATH (something like "C:\Qt\Qt5.6.2\5.6\msvc2015\bin"), so the programs will be able to located the necessary DLL files.
