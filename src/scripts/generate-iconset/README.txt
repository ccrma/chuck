macOS
---
python3 generate-iconset.py ~/Desktop/chuck.png

this will:
1. create a folder ~/Desktop/chuck.iconset
2. generate 10 files with different dimensions and save those into that folder

next (from Desktop):
iconutil -c icns chuck.iconset -o chuck.icns

this will:
create a single chuck.icns file


windows
---
magick.exe convert icon-16.png icon-32.png icon-256.png icon.ico

--- author and reference ---
Old version (we are using this one)
https://github.com/retifrav/python-scripts/tree/master/generate-iconset

New version (python package):
https://retifrav.github.io/blog/2018/10/09/macos-convert-png-to-icns/
https://github.com/retifrav/generate-iconset/

