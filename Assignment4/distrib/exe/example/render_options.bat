@echo off

set inputfile=%1
echo %inputfile%
set scenename=%inputfile:.txt=%
rem depth must be the last argument
..\..\bin\example_x64 -input %* %scenename%_dep_example.png -output %scenename%_rgb_example.png -normals %scenename%_nor_example.png
move %scenename%_rgb_example.png ..\out\
move %scenename%_dep_example.png ..\out\
move %scenename%_nor_example.png ..\out\
