SET CWD=%CD% 
cd %CWD%/../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_twrk24f120m 
cmd /c build_release nopause
cd %CWD%/../../../../../build/armgcc/mfs_twrk24f120m 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_twrk24f120m 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_twrk24f120m 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../lib/ksdk_mqx_lib/armgcc/K24F25612 
cmd /c build_release nopause
cd %CWD% 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash release"  -G "MinGW Makefiles" 
mingw32-make all 
if not "%1" == "nopause" (pause) 
