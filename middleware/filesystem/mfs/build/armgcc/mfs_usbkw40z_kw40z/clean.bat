DEL /s /q /f *.map 
DEL /s /q /f Makefile 
DEL /s /q /f cmake_install.cmake 
DEL /s /q /f CMakeCache.txt 
RD /s /q CMakeFiles 
RD /s /q "debug" 
RD /s /q "release" 
if not "%1" == "nopause" (pause) 
