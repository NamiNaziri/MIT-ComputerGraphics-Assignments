@echo off

:begin
if "%1"=="" goto end
rem the argument may contain the full path...
set inputfile=%1
echo %inputfile%
set scenename=%inputfile:.txt=%
..\..\bin\example_x64 -input %inputfile% -size 200 200 -output %scenename%_rgb_example.png -depth 0 20 %scenename%_dep_example.png -normals %scenename%_nor_example.png
move %scenename%_rgb_example.png ..\out\
move %scenename%_dep_example.png ..\out\
move %scenename%_nor_example.png ..\out\
shift
goto begin

:end
