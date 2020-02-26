@RD /S /Q "C:\respaldo2017\RomsXShop\build"
xcopy "C:\respaldo2017\C++\RomsXShop\test\*.cpp" "C:\respaldo2017\RomsXShop\source\*.cpp" /y
make
nxlink -s "out/RomsXShop.nro" 