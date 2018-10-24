SET CWD=%CD% 
cd %CWD%/../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_frdmkv31f 
cmd /c build_release nopause
cd %CWD%/../../../../../build/armgcc/mfs_frdmkv31f 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_frdmkv31f 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_frdmkv31f 
cmd /c build_release nopause
cd %CWD%/../../../../../../../../lib/ksdk_mqx_lib/armgcc/KV31F51212 
cmd /c build_release nopause
cd %CWD% 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash release"  -G "MinGW Makefiles" 
mingw32-make all 
if not "%1" == "nopause" (pause) 
