SET CWD=%CD% 
cd %CWD%/../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_twrk24f120m 
cmd /c build_debug nopause
cd %CWD%/../../../../../build/armgcc/mfs_twrk24f120m 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_twrk24f120m 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_twrk24f120m 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../lib/ksdk_mqx_lib/armgcc/K24F25612 
cmd /c build_debug nopause
cd %CWD%/../../../../../../../../usb/usb_core/host/lib/mqx/armgcc/MK24F25612 
cmd /c build_debug nopause
cd %CWD% 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash debug"  -G "MinGW Makefiles" 
mingw32-make all 
if not "%1" == "nopause" (pause) 
