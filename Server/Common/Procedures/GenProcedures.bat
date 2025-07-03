pushd %~dp0


GenProcedures.exe --path=../../DBServer/GameDB.xml --output=GenProcedures.h
IF ERRORLEVEL 1 PAUSE

XCOPY /Y GenProcedures.h "../../DBServer"

DEL /Q /F *.h

PAUSE