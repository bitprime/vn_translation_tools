@echo off
mkdir Scene\SceneNew  >nul 2>&1
for %%s in (Scene\\*.ss) do ssinsert-v200.exe "%%s" "Scene\%%~ns-str.txt" "Scene\SceneNew\%%~nxs"
copy /Y Scene\vars.dat Scene\SceneNew\vars.dat >nul 2>&1
copy /Y Scene\vars1.dat Scene\SceneNew\vars1.dat >nul 2>&1
copy /Y Scene\vars2.dat Scene\SceneNew\vars2.dat >nul 2>&1
copy /Y Scene\vars3.dat Scene\SceneNew\vars3.dat >nul 2>&1
@echo Finished inserting scripts
createpck.exe "Scene\SceneNew" "Scene.pck" -c -e
@echo on
@echo Finished packing new Scene.pck
pause