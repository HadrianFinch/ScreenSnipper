@echo off

taskkill /im screensnipper.exe /T /F

echo[

echo compiling resources...
echo *
rc .\resources.rc
echo *
echo resources compiled

echo[

echo building files ... 

echo *

cl.exe /W0 /Zi /EHsc /I .\packages /nologo /Fe: .\ScreenSnipper.exe .\src\*.cpp /link /SUBSYSTEM:WINDOWS .\resources.res

@if ERRORLEVEL == 0 (
   goto good
)

@if ERRORLEVEL != 0 (
   goto bad
)

:good
echo *
echo build complete

echo[

echo attaching manafest ...

mt.exe -manifest .\ScreenSnipper.exe.manifest -outputresource:.\ScreenSnipper.exe;1

echo manafest attached

echo[

echo Copying to CODECARRIER...
xcopy .\ScreenSnipper.exe E:\ /Y /Q /S

echo copy complete

goto end




:bad
echo *
echo compile error. aborting.

:end