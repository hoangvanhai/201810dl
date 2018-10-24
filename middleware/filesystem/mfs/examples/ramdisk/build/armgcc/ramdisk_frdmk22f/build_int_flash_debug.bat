SET CWD=%CD% 
cd %CWD%/../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_frdmk22f 
cmd /c build_debug nopause
cd %CWD%/../../../../../build/armgcc/mfs_frdmk22f 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_frdmk22f 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_frdmk22f 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../lib/ksdk_mqx_lib/armgcc/K22F51212 
cmd /c build_debug nopause
cd %CWD% 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash debug"  -G "MinGW Makefiles" 
mingw32-make all 
if not "%1" == "nopause" (pause) 
