# Copyright (C) 2016 by Ewan Barr
# Licensed under the Academic Free License version 3.0
# This program comes with ABSOLUTELY NO WARRANTY.
# You are free to modify and redistribute this code as long
# as you do not remove the above attribution and reasonably
# inform receipients that you have modified the original work.

FROM nvidia/cuda:8.0-devel-ubuntu16.04

MAINTAINER Ewan Barr <ewan.d.barr@gmail.com>

# Suppress debconf warnings
ENV DEBIAN_FRONTEND noninteractive

# Create space for ssh daemon and update the system
RUN echo 'deb http://us.archive.ubuntu.com/ubuntu trusty main multiverse' >> /etc/apt/sources.list && \
    apt-get -y check && \
    apt-get -y update && \
    apt-get install -y apt-utils apt-transport-https software-properties-common python-software-properties && \
    apt-get -y update --fix-missing && \
    apt-get -y upgrade && \
    apt-get -y install gdb

# Pick up some MOFED dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    net-tools \
    ethtool \
    perl \
    lsb-release \
    iproute2 \
    pciutils \
    libnl-route-3-200 \
    kmod \
    libnuma1 \
    lsof \
    gdb \
    linux-headers-4.4.0-92-generic \
    python-libxml2 && \
    rm -rf /var/lib/apt/lists/*

# Download and install Mellanox OFED 4.1.1 for Ubuntu 16.04
RUN wget http://content.mellanox.com/ofed/MLNX_OFED-4.1-1.0.2.0/MLNX_OFED_LINUX-4.1-1.0.2.0-ubuntu16.04-x86_64.tgz && \
    tar -xzvf MLNX_OFED_LINUX-4.1-1.0.2.0-ubuntu16.04-x86_64.tgz && \
    MLNX_OFED_LINUX-4.1-1.0.2.0-ubuntu16.04-x86_64/mlnxofedinstall --user-space-only --without-fw-update --all -q && \
    cd .. && \
    rm -rf MLNX_OFED_LINUX-4.1-1.0.2.0-ubuntu16.04-x86_64 && \
    rm -rf *.tgz

# Switch account to root and adding user accounts and password
USER root

# Install dependencies
RUN apt-get update &&\
    apt-get --no-install-recommends --allow-unauthenticated -y install \
    apt-transport-https \
    apt-utils \
    software-properties-common \
    build-essential \
    autoconf \
    autotools-dev \
    automake \
    autogen \
    libtool \
    libfftw3-3 \
    libfftw3-bin \
    libfftw3-dev \
    libfftw3-single3 \
    tcpdump \
    csh \
    gcc \
    gfortran \
    wget \
    git \
    cvs \
    cmake \
    vim \
    gdb \
    tmux \
    net-tools \
    expect \
    libcfitsio-dev \
    libltdl-dev \
    gsl-bin \
    libgsl-dev \
    libgsl2 \
    hwloc \
    libhwloc-dev \
    libboost-all-dev \
    pkg-config

# Define home, psrhome, OSTYPE and create the directory
ENV PSRHOME /software/
ENV OSTYPE linux
RUN mkdir -p $PSRHOME
WORKDIR $PSRHOME

# Install PSRDADA
RUN git clone git://git.code.sf.net/p/psrdada/code psrdada
ENV PSRDADA_HOME $PSRHOME/psrdada
WORKDIR $PSRDADA_HOME
COPY PsrdadaMakefile.am $PSRDADA_HOME/Makefile.am
RUN mkdir build/ && \
    ./bootstrap && \
    ./configure --prefix=/usr/local && \
    make && \
    make install && \
    make clean 
ENV PSRDADA_BUILD $PSRHOME
ENV PACKAGES $PSRDADA_BUILD

# Install SPEAD2
WORKDIR $PSRHOME
RUN git clone https://github.com/ska-sa/spead2.git && \
    cd spead2 && \
    ./bootstrap.sh --no-python && \
    ./configure --prefix=/usr/local && \
    make -j 6 && \
    make install && \
    make clean 
ENV PACKAGES $PACKAGES:$PSRHOME

ENV ARSE 3 

#install PSRDADA_CPP
WORKDIR $PSRHOME
RUN git clone https://github.com/ewanbarr/psrdada_cpp && \
    cd psrdada_cpp && \
    git checkout transpose && \
    git pull origin transpose && \
    mkdir build && \
    cd build && \
    cmake -DENABLE_CUDA=false -DCMAKE_INSTALL_PREFIX=/usr/local ../ && \
    make -j 1 && \
    make install && \

#install MKRecv
WORKDIR $PSRHOME
RUN git config --global http.sslverify false && \
    git clone https://gitlab.mpifr-bonn.mpg.de/mhein/mkrecv.git && \
    cd mkrecv && \
    git checkout master && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j 1 && \
    make install

# Install all the cheetah pipeline dependencies

RUN cd /software/ && \
    git clone https://gitlab.com/SKA-TDT/cheetah.git && \
    git clone https://gitlab.com/SKA-TDT/panda.git


#Install PANDA
RUN cd /software/panda/ && \
    mkdir build/ && \
    cd build/ && \
    cmake -DENABLE_CUDA=true ../ && \
    make -j 16 && \
    make install

#Install Astro-Accelerate
RUN cd /software/ && \
    git clone https://github.com/AstroAccelerateOrg/astro-accelerate.git && \
    cd /software/astro-accelerate/ && \
    git checkout aa_interface && \
    mkdir build/ && \
    cd build/ && \
    cmake -DENABLE_CUDA=true ../ && \
    make -j 16 && \
    make install

# Install cheetah
RUN cd /software/cheetah/ && \
    git checkout dev && \
    mkdir build/ && \
    cd build/ && \
    cmake -DENABLE_CUDA=true -DENABLE_PSRDADA=true ../ && \
    make -j 16
