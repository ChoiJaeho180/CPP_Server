pushd %~dp0


GenCmsInit.exe --cms_path=../../Cms --template=CmsManager.cpp --output=../../GameServer/CmsManager.cpp --header_path=../../GameServer --vcxproj_path=../../GameServer/GameServer.vcxproj --vcxproj_filter_path=../../GameServer/GameServer.vcxproj.filters --filter=Cms\Description
IF ERRORLEVEL 1 PAUSE

XCOPY /Y CmsManager.cpp "../../GameServer"

DEL /Q /F *.h

PAUSE