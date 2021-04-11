@echo off

taskkill /im screensnipper.exe /T /F

echo[

echo compiling resources...
rc .\resources.rc
echo resources compiled

echo[

echo building files ... 

echo *

cl.exe /Zi /EHsc /nologo /Fe: .\ScreenSnipper.exe .\src\*.cpp /link /SUBSYSTEM:WINDOWS .\resources.res

echo *
echo build complete

echo[

echo attaching manafest ...

mt.exe -manifest .\ScreenSnipper.exe.manifest -outputresource:.\ScreenSnipper.exe;1

echo manafest attached

echo[

echo Copying to CODECARRIER...
xcopy .\*.* E:\ /Y /Q /S

echo copy complete