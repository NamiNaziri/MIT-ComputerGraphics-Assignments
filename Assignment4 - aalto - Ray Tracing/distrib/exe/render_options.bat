@echo off

set inputfile=%1
echo %inputfile%
set scenename=%inputfile:.txt=%
rem depth must be the last argument
..\bin\assignment_x64_release -input %* %scenename%_dep.png -output %scenename%_rgb.png -normals %scenename%_nor.png
move %scenename%_rgb.png out\
move %scenename%_dep.png out\
move %scenename%_nor.png out\
