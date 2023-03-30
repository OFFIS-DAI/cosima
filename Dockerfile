FROM ubuntu:20.04
MAINTAINER Malin Radtke
LABEL Description="Docker image for cosima project containing python installation and OMNeT++ 5.6.2"

# Set the env variable DEBIAN_FRONTEND to noninteractive
ENV DEBIAN_FRONTEND noninteractive

# Install dependencies
RUN apt-get update && apt-get install -qq -y \
    python \
    perl-base \
    cmake \
    python3 \
    python3-sphinx \
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

# install google test
RUN git clone https://github.com/google/googletest.git -b release-1.11.0
RUN cd googletest && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    cd .. && \
    cd ..

RUN apt update && apt install  openssh-server sudo -y
RUN  echo 'root:password' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/g' /etc/ssh/sshd_config
RUN service ssh start
EXPOSE 22
CMD ["/usr/sbin/sshd","-D"]

# Regenerate SSH host keys. baseimage-docker does not contain any, so you
# have to do that yourself. You may also comment out this instruction; the
# init system will auto-generate one during boot.
# RUN /etc/my_init.d/00_regen_ssh_host_keys.sh

# Copied command from https://github.com/rogaha/docker-desktop/blob/master/Dockerfile
# Configuring xdm to allow connections from any IP address and ssh to allow X11 Forwarding.
RUN sed -i 's/DisplayManager.requestPort/!DisplayManager.requestPort/g' /etc/X11/xdm/xdm-config
RUN sed -i '/#any host/c\*' /etc/X11/xdm/Xaccess
RUN ln -s /usr/bin/Xorg
RUN echo X11Forwarding yes >> /etc/ssh/ssh_config

# OMnet++ 5.6.2

# Create working directory
RUN mkdir -p /usr/omnetpp
WORKDIR /usr/omnetpp

# Fetch Omnet++ source
RUN wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.6.2/omnetpp-5.6.2-src-linux.tgz
RUN tar -xf omnetpp-5.6.2-src-linux.tgz

# Path
ENV PATH $PATH:/usr/omnetpp/omnetpp-5.6.2/bin

# Configure and compile
RUN cd omnetpp-5.6.2 && \
    xvfb-run ./configure PREFER_CLANG=yes CXXFLAGS=-std=c++14 && \
    make

# Cleanup
RUN apt-get clean && \
    rm -rf /var/lib/apt && \
    rm /usr/omnetpp/omnetpp-5.6.2-src-linux.tgz

# INET
RUN mkdir -p /root/models
WORKDIR /root/models
RUN wget https://github.com/inet-framework/inet/releases/download/v4.2.2/inet-4.2.2-src.tgz \
    && tar -xzf inet-4.2.2-src.tgz \
    && rm inet-4.2.2-src.tgz 
#    && mv inet4 inet
WORKDIR /root/models/inet4
RUN make makefiles \
    && make -j$(grep -c proc /proc/cpuinfo) \
    && make MODE=release -j$(grep -c proc /proc/cpuinfo)
# Import inet into eclipse workspace
    # && /usr/omnetpp/omnetpp-5.6.2 -nosplash -data /root/models -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import /root/models/inet
ARG VERSION=4.2.2
ENV INET_VER=$VERSION
RUN echo 'PS1="inet-$INET_VER:\w\$ "' >> /root/.bashrc && chmod +x /root/.bashrc && \
    touch /root/.hushlogin

# SimuLTE
WORKDIR /root/models
RUN wget https://github.com/inet-framework/simulte/releases/download/v1.2.0/simulte-1.2.0-src.tgz
RUN tar -xzf simulte-1.2.0-src.tgz && \
    rm simulte-1.2.0-src.tgz
WORKDIR /root/models/simulte
RUN make makefiles \
    && make -j$(grep -c proc /proc/cpuinfo) \
    && make MODE=release -j$(grep -c proc /proc/cpuinfo)

# install protobuf
RUN apt-get update -y && apt-get install -y libprotobuf-dev protobuf-compiler

RUN apt-get update && apt-get install -y python3.8 python3.8-dev\
    python3-pip

WORKDIR /root/models
# copy files and install pip requirements
COPY . .
RUN pip3 install -r requirements.txt

# Build OMNeT++ files
WORKDIR /root/models/cosima_omnetpp_project
RUN make -f makemakefiles
RUN make


