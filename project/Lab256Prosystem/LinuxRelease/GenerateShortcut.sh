#!/bin/bash


cp -a Lab256runorg.sh Lab256run.sh

sed -i "s!CURRENTPATH!`pwd`!g" Lab256run.sh


:> ./Lab256Pro.desktop

echo [Desktop Entry] >> ./Lab256Pro.desktop

curpath=`pwd`
echo Name=LAB256Pro ${curpath##*/} >> ./Lab256Pro.desktop

echo Comment=LAB256 Prosystem >> ./Lab256Pro.desktop

echo Exec=`pwd`/Lab256run.sh >> ./Lab256Pro.desktop

echo Icon=`pwd`/BGI_LOGO.ico >> ./Lab256Pro.desktop

echo Terminal=false >> ./Lab256Pro.desktop

echo Type=Application >> ./Lab256Pro.desktop

echo Encoding=UTF-8 >> ./Lab256Pro.desktop


chmod +x Lab256Prosystem
chmod +x Lab256run.sh
cp -a Lab256Pro.desktop ~/Desktop/
chmod +x  ~/Desktop/Lab256Pro.desktop


#echo `date +"%Y-%m-%d %H:%M:%S"` begin >> ./a.log

