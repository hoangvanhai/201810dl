SET CWD=%CD% 
cd %CWD%/../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_twrk60d100m 
cmd /c build_release nopause
cd %CWD%/../../../../../build/armgcc/mfs_twrk60d100m 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_twrk60d100m 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_twrk60d100m 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../lib/ksdk_mqx_lib/armgcc/K60D10 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../usb/usb_core/host/lib/mqx/armgcc/MK60D10 
cmd /c build_release nopause
cd %CWD% 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash release"  -G "MinGW Makefiles" 
mingw32-make all 
if not "%1" == "nopause" (pause) 
