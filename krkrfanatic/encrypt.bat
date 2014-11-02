mkdir encrypted
for %%s in (*.tjs) do  krkrfanatic.exe -e "%%s" "encrypted\\%%s"
for %%s in (*.ks) do  krkrfanatic.exe -e "%%s" "encrypted\\%%s"
for %%s in (*.scn) do  krkrfanatic.exe -e "%%s" "encrypted\\%%s"