# acscontroller

## .cpp file compilation in Windows OS:

Use mingw64 (C:\msys64) to compile. Follow https://code.visualstudio.com/docs/cpp/config-mingw for install instructions.

    $ g++ C:/path_to_repository/acscontroller.cpp C:/path_to_repository/stage.cpp -o C:/path_to_repository/stage.exe -LC:/path_to_repository/C_CPP/Dependencies -lACSCL_x64 -mconsole