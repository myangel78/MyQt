#!/bin/bash


#sudo -a sources.list /etc/apt/

sudo apt update

#cd /..?
#sudo wget ...?
#sudo apt upgrade

#sudo apt install -y m4

sudo apt install -y software-properties-common
sudo add-apt-repository ppa:ubuntu-toolchain-r/test

sudo apt install -y gcc-10
sudo apt install -y g++-10

#sudo apt install -y tightvncserver xrdp
#sudo apt install -y xubuntu-desktop

sudo apt install -y openssh-server
sudo apt install -y  vim-gtk   git  tree  htop
sudo apt install -y gcc make build-essential libssl-dev zlib1g-dev libbz2-dev libreadline-dev libsqlite3-dev wget curl llvm libncurses5-dev libncursesw5-dev xz-utils tk-dev libffi-dev liblzma-dev
sudo apt install -y pkg-config libhdf5-100 libhdf5-dev
sudo apt install -y redis-server
#sudo apt upgrade

sudo apt-get install -y kazam
#sudo apt-get install -y smplayer
#sudo dpkg -i wps-office_11.1.0.10161_amd64.deb

sudo cp -a libcurl.so.4.7.0 /usr/lib/x86_64-linux-gnu/
sudo ln -sf /usr/lib/x86_64-linux-gnu/libcurl.so.4.7.0 /usr/lib/x86_64-linux-gnu/libcurl.so.4

sudo cp -a libhiredis.so /usr/lib/x86_64-linux-gnu/
sudo ln -sf /usr/lib/x86_64-linux-gnu/libhiredis.so /usr/lib/x86_64-linux-gnu/libhiredis.so.1.0.1-dev


#sudo mkfs.ext4 /dev/sda
#sudo mkdir /data
#sudo mkdir /data/raw_data
#sudo mkdir /data/output_data
#echo "/dev/sda  /data    ext4    defaults 0 0" >> /etc/fstab
#sudo mount -a
#sudo chown -R shanzhu:shanzhu /data

#sudo mkfs.ext4 /dev/sda
#sudo mkdir /data
#echo "/dev/sda  /data    ext4    defaults 0 0" >> /etc/fstab
#sudo mount -a
#cd /data
#sudo mkdir raw_data
#sudo mkdir output_data
#sudo chown -R shanzhu /data

#ln -sf /data /home/shanzhu/Desktop/data
