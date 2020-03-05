packfolder.exe ui resources.cpp -v "resources"
msbuild "Random Photo.sln" /p:Configuration=Debug /p:Platform=x86 -m
cd Debug
"Random Photo.exe"