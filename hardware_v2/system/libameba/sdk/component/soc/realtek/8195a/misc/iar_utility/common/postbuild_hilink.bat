set tooldir=%1\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools
set hilinkdir=%1\..\..\..\component\common\application\hilink

::echo %tooldir% > %hilinkdir%\test.txt
::echo %hilinkdir% >> %hilinkdir%\test.txt

cd /D %tooldir%

::echo "%tooldir%\iarchive.exe --extract %hilinkdir%\libhilinkdevicesdk.a" >> %hilinkdir%\test.txt

echo cmd /c iarchive.exe --create lib_hilink.a  >out.bat

cmd /c "iarchive.exe -t %hilinkdir%\lib_porting.a" >>out.bat
cmd /c "iarchive.exe -t %hilinkdir%\libhilinkdevicesdk.a" >>out.bat
cmd /c sed ':a;N;$ s/\n/ /g;ba' out.bat > out1.bat

cmd /c "iarchive.exe --extract %hilinkdir%\lib_porting.a"
cmd /c "iarchive.exe --extract %hilinkdir%\libhilinkdevicesdk.a"

cmd /c "out1.bat"

del %hilinkdir%\lib_porting.a
cmd /c copy lib_hilink.a %hilinkdir%

del *.o
del *.bat
del *.a 

exit
