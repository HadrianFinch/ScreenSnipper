@echo off

taskkill /im screensnipper.exe

echo building files ...

echo *

cl.exe /Zi /EHsc /nologo /Fe: .\ScreenSnipper.exe .\src\*.cpp

echo *
echo build complete
echo attaching manafest ...

mt.exe -manifest .\ScreenSnipper.exe.manifest -outputresource:.\ScreenSnipper.exe;1

echo manafest attached

echo Copying to CODECARRIER...
xcopy .\*.* E:\ /Y /Q /S

echo copy complete