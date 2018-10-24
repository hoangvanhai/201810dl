cmd /c .\build_debug.bat nopause 
cmd /c .\build_release.bat nopause 
if not "%1" == "nopause" (pause) 
