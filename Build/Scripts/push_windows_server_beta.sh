#!/bin/bash
mkdir ../butler
rm -rfv ../butler/VC
cp -r ../WindowsServer/ ../butler/VC
rm -rfv ../butler/VC/VC/Saved
rm -rfv ../butler/VC/FileOpenOrder
rm -rfv ../butler/VC/Manifest*.txt
./butler.exe push --ignore '*.pdb' ../butler/ epicgameguy/vc:windows-server
read -p "Press any key to close window..."
