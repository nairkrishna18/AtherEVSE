@echo off

title FOTA LOADER (KRN)
echo Author : Krishna Raveendran Nair
echo CerDate   : 18/06/2022
echo ModDate   : 15/08/2023
echo ...Modified Path to test FOTA Functionality....

echo .........Copy --> Rename --> firmware.bin!

if exist "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder\ModFirmwareFota.bin" (
echo .........Deleting Previous File!
del D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder\ModFirmwareFota.bin
) else (echo .........ModFirmwareFota.bin File Not Available To Delete)

echo .........Copying firmware.bin File to FotaFolder........
rem Only as a commment.......... This line will not be printed........
copy "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\.pio\build\esp32doit-devkit-v1\firmware.bin" "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder"

echo .........Renaming firmware.bin Files to ModFirmwareFota.bin........
rename "D:\PROJECTS\ECHOTRONICS\AtherEVSE\AtherEVSE_V1\FotaFolder\firmware.bin" ModFirmwareFota.bin
echo .........Renaming Completed........
echo .........Process Completed.........

pause