:: change work directory to tool directory
cd /d %1

set tooldir=tools
set libdir=bsp

:: copy application.axf to current directory
xcopy /y %2 .\

if exist application.map del application.map
if exist application.asm del application.asm
if exist *.bin del *.bin

cmd /c "%tooldir%\nm application.axf | %tooldir%\sort > application.map"
cmd /c "%tooldir%\objdump -d application.axf > application.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __ram_image2_text  application.map | %tooldir%\grep _start__ | %tooldir%\gawk '{print $1}'"') do set ram2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __sdram_data_      application.map | %tooldir%\grep start__  | %tooldir%\gawk '{print $1}'"') do set ram3_start=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __ram_image2_text  application.map | %tooldir%\grep _end__   | %tooldir%\gawk '{print $1}'"') do set ram2_end=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __sdram_data_      application.map | %tooldir%\grep end__    | %tooldir%\gawk '{print $1}'"') do set ram3_end=0x%%i

::echo. > tmp.txt
::echo %ram2_start% >> tmp.txt
::echo %ram3_start% >> tmp.txt
::echo %ram2_end% >> tmp.txt
::echo %ram3_end% >> tmp.txt

cmd /c "%tooldir%\objcopy.exe -j .image2.start.table -j .ram_image2.text -j .ram.data -Obinary ./application.axf ./ram_2.bin
if %ram3_end% NEQ  %ram3_start% (
	:: TODO: need check which sections belong to sdram
	cmd /c "%tooldir%\objcopy.exe -j .image3 -j .ARM.exidx -j .sdr_data -Obinary ./application.axf ./sdram.bin
)

%tooldir%\pick %ram2_start% %ram2_end% ram_2.bin ram_2.p.bin body+reset_offset+sig
%tooldir%\pick %ram2_start% %ram2_end% ram_2.bin ram_2.ns.bin body+reset_offset
if defined %ram3_start (
	%tooldir%\pick %ram3_start% %ram3_end% sdram.bin ram_3.p.bin body+reset_offset
)

copy bsp\image\ram_1.p.bin .

::padding ram_1.p.bin to 32K+4K+4K+4K, LOADER/RSVD/SYSTEM/CALIBRATION
%tooldir%\padding 44k 0xFF ram_1.p.bin

:: SDRAM case
if %ram3_end% NEQ %ram3_start% (
	copy /b ram_1.p.bin+ram_2.p.bin+ram_3.p.bin ram_all.bin
	copy /b ram_2.ns.bin+ram_3.p.bin ota.bin
)

:: NO SDRAM case
if %ram3_end% EQU %ram3_start% (
	copy /b ram_1.p.bin+ram_2.p.bin ram_all.bin
	copy /b ram_2.ns.bin ota.bin
	set ram3_end=0xFFFFFFFF
)

%tooldir%\checksum ota.bin


