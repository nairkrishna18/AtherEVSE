@echo off

title FOTA LOADER (KRN)

rem To add new line use %n% wherever required
set n=^&echo.

echo Author : Krishna Raveendran Nair %n%
echo Created  Date   : 18/06/2022
echo Modified Date   : 22/09/2023
echo 22/09/23 ...Open Git Hub Link in, your Browser to Paste Firmware.bin file....
echo 15/08/23 ...Modified Paths to test FOTA Functionality....%n%

echo .........[Copy] -- [Rename] -- firmware.bin! %n%

if exist "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder\ModFirmwareFota.bin" (
echo .........Deleting Previous File!%n%
del D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder\ModFirmwareFota.bin
) else (echo .........ModFirmwareFota.bin File Not Available To Delete.%n%)

echo .........Copying firmware.bin File to FotaFolder........%n%
rem Only as a commment.......... This line will not be printed........
copy "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\.pio\build\esp32doit-devkit-v1\firmware.bin" "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder"

echo:

echo .........Renaming firmware.bin Files to ModFirmwareFota.bin........
rename "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder\firmware.bin" ModFirmwareFota.bin
echo .........Renaming Completed........%n%

rem (start "" url/link ) is used to open any website link in your browser.....
echo .........Opening Git Hub Server Location To Paste Firmware.bin File........%n%
start "" https://github.com/nairkrishna18/AtherEVSE/tree/main/FotaFolder

echo .........Process Completed.........%n%
pause