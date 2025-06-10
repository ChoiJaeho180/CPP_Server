pushd %~dp0

pyinstaller --onefile CmsInitialGenerator.py

MOVE .\dist\CmsInitialGenerator.exe .\GenCmsInit.exe
@RD /S /Q .\build
@RD /S /Q .\dist

DEL /S /F /Q .\CmsInitialGenerator.spec
PAUSE