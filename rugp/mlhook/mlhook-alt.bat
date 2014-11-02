gcc -shared -static mlhook-alt.cpp -o mlhook-alt.dll -Os -s -masm=intel -lz
copy /Y mlhook-alt.dll "C:\age\マブラヴオルタネイティヴ\mlhook.dll"
pause