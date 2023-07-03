#!/bin/bash
# This is an installation script for the cosima project
# The script works under ubuntu

workdir=$(pwd)
echo "-------------------------------------------------------------"
echo "Working directory is $workdir"

echo "-------------------------------------------------------------"
echo "Installing OMNeT++"
# install dependencies for OMNeT++
sudo apt-get update
sudo apt-get install -y python \
    python3 \
    python3-sphinx \
    perl-base \
    cmake \
    libwebkit2gtk-4.0-37 \
    libwebkit2gtk-4.0-37-gtk2 \
    qtchooser libqt5core5a \
    openscenegraph-plugin-osgearth \
    libosgearth-dev \
    openmpi-bin \
    libopenmpi-dev \
    xpra\
    rox-filer\
    openssh-server\
    pwgen\
    xserver-xephyr\
    xdm\
    fluxbox\
    sudo\
    git \
    xvfb\
    wget \
    build-essential \
    gcc \
    g++\
    bison \
    flex \
    perl \
    qt5-default\
    tcl-dev \
    tk-dev \
    libxml2-dev \
    zlib1g-dev \
    default-jre \
    doxygen \
    graphviz \
    ssh \
    curl \
    clang \
    libgtest-dev

# go to home directory
cd ~
# install OMNet++ 5.6.2
# Create working directory
mkdir -p omnetpp
cd omnetpp
# Fetch Omnet++ source
wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.6.2/omnetpp-5.6.2-src-linux.tgz
tar -xf omnetpp-5.6.2-src-linux.tgz
# Path
export PATH=$PATH:/usr/omnetpp/omnetpp-5.6.2/bin
# Configure and compile
cd omnetpp-5.6.2
./configure PREFER_CLANG=yes
make

echo "-------------------------------------------------------------"
echo "Installing INET"
# install INET
cd "$workdir"
# download files
wget https://github.com/inet-framework/inet/releases/download/v4.2.2/inet-4.2.2-src.tgz
tar -xzf inet-4.2.2-src.tgz
rm inet-4.2.2-src.tgz
mv inet4 inet
cd inet
make makefiles

echo "-------------------------------------------------------------"
echo "Installing protobuf"
# install protobuf
sudo apt-get update
sudo apt-get install -y libprotobuf-dev protobuf-compiler

echo "-------------------------------------------------------------"
echo "Installing python3.8"
# install python
sudo apt-get update
sudo apt-get install -y python3.8 \
     python3-pip

echo "-------------------------------------------------------------"
echo "Installing pip requirements"
# install python pip requirements
cd "$workdir"
pip install virtualenv
virtualenv venv
source venv/bin/activate
pip install -r requirements.txt
pip install testresources


echo "-------------------------------------------------------------"
echo "Everything is installed. Now you can import your project files into the OMNeT++ workspace."

