cmd /c .\build_int_flash_debug.bat nopause 
cmd /c .\build_int_flash_release.bat nopause 
if not "%1" == "nopause" (pause) 
