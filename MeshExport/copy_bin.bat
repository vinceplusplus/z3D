@echo off

if /i '%1' EQU 'dbg' goto dbg
if /i '%1' EQU 'rel' goto rel
goto error

:dbg
if exist copy_dbg.bat goto copy_dbg
echo.
echo ERROR: copy_dbg.bat is missing
echo.
echo SAMPLE of copy_dbg.bat
echo copy "..\bin_dbg\MeshExport.dle" "D:\3dsMax9\plugins"
echo copy "..\bin_dbg\z3DCoreCrit.dll" "D:\3dsMax9"
echo copy "..\bin_dbg\z3DCoreUIHelper.dll" "D:\3dsMax9"
echo copy "..\bin_dbg\nedmalloc.dll" "D:\3dsMax9"
goto end

:rel
if exist copy_rel.bat goto copy_rel
echo.
echo ERROR: copy_rel.bat is missing
echo.
echo SAMPLE of copy_rel.bat
echo copy "..\bin\MeshExport.dle" "D:\3dsMax9\plugins"
echo copy "..\bin\z3DCoreCrit.dll" "D:\3dsMax9"
echo copy "..\bin\z3DCoreUIHelper.dll" "D:\3dsMax9"
echo copy "..\bin\nedmalloc.dll" "D:\3dsMax9"
goto end

:error
echo ERROR: PLEASE specify either DBG or REL as the first parameter
goto end

:copy_dbg
call copy_dbg.bat
goto end

:copy_rel
call copy_rel.bat
goto end

:end
