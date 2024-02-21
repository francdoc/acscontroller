# ACS control software

Compilation examples for Windows:
 
* g++ .\acscontroller.cpp .\stage.cpp .\stagetest.cpp -o stagetest.exe -L C:.\C_CPP\Dependencies\ -lACSCL_x64 -mconsole -lws2_32
* ./stagetest
* g++ .\serversocket.cpp .\servertest.cpp -o servertest.exe -mconsole -lws2_32
* ./servertest 1000
* g++ .\acscontroller.cpp .\stage.cpp .\windowsnet\serversocket.cpp .\stageserver.cpp .\stageservertest.cpp -o stageservertest.exe -L C:.\C_CPP\Dependencies\ -lACSCL_x64 -mconsole -lws2_32
* ./stageservertest 1000
* Video tutorial for compiling: https://www.youtube.com/watch?v=oC69vlWofJQ&t=86s