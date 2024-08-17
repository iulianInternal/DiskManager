#!/bin/bash
echo This script will create an icon to the program.
echo You are not required to run this script.
echo But you may look at this code of script.
echo
echo Requires to run this script as not superuser.
echo Some of installed programs that this script uses \(if you have it\)
echo just refuses to run as superuser.
echo
echo You have to run this script within executable directory.
echo
echo Running script as $USER \($( [ "${SUDO_USER}" != "" ] && echo running as superuser || echo running as not superuser)\)
echo
echo To continue running, say \"Yes, I need it!\"
echo otherwise press CTRL + C to interupt script.
warning=""
while [ "$warning" != "Yes, I need it!" ]; do
	read warning
done

#Creating icons for current user if not done before
if [ ! -d /home/$USER/.local/share/icons ]; then
	mkdir /home/$USER/.local/share/icons
fi
if [ ! -d /home/$USER/.local/share/icons/hicolor ]; then
	mkdir /home/$USER/.local/share/icons/hicolor
fi
Icons=(16 24 32 48 64)
for i in "${Icons[@]}"; do
	if [ ! -d ~/.local/share/icons/hicolor/${i}x${i} ]; then
		mkdir ~/.local/share/icons/hicolor/${i}x${i}
	fi
	if [ ! -d ~/.local/share/icons/hicolor/${i}x${i}/apps ]; then
		mkdir ~/.local/share/icons/hicolor/${i}x${i}/apps
	fi
	cp ./assets/DiskManager@${i}x${i}.png ~/.local/share/icons/hicolor/${i}x${i}/apps/DiskManager.png
	echo Added DiskManager@${i}x${i}.png to local icon folder.
done

Executable=${PWD//\ /\\ }
echo "[Desktop Entry]
Name=DiskManager
Type=Application
Exec=bash -c \"$Executable/DiskManager %F; cd $Executable; bash -i\"
Comment=A program that allows to read/write raw image files of a diskette.
Terminal=true
Icon=DiskManager" > /home/$USER/.local/share/applications/DiskManager.desktop
echo Added desktop entry to local applications folder

#Manually adding icon to a file manager (GNOME Files)
if [ "$(command -v gio)" ]; then
	gio set "$PWD/DiskManager" metadata::custom-icon "file:///home/$USER/.local/share/icons/hicolor/64x64/apps/DiskManager.png"
	echo Added metadata to executable for GNOME Files.
fi

echo Done. Icons are set.
echo If you are running a program from application menu,
echo it is recommended that you in your terminal settings
echo set option 'Keep terminal after command execution' for debugging reasons.
