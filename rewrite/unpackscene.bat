@echo off
parsepck.exe Scene.pck Scene
@echo Finished extracting Scene.pck
for %%s in (Scene\\*.ss) do ssdump-v200.exe "%%s" "Scene\%%~ns-str.txt"
@echo Finished dumping scripts
@echo on
pause