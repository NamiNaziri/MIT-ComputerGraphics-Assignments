@echo off

:begin
if "%1"=="" goto end
rem the argument may contain the full path...
set inputfile=%1
echo %inputfile%
set scenename=%inputfile:.txt=%
..\bin\assignment_x64_release -input %inputfile% -size 200 200 -output %scenename%_rgb.png -depth 0 20 %scenename%_dep.png -normals %scenename%_nor.png
move %scenename%_rgb.png out\
move %scenename%_dep.png out\
move %scenename%_nor.png out\
shift
goto begin

:end
