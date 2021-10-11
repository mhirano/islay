#FROM nvidia/cuda:11.4.1-cudnn8-runtime-ubuntu20.04
FROM nvidia/cuda:11.1.1-cudnn8-devel-ubuntu18.04

# tzdata is to suppress time-zone selection prompt
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Tokyo
RUN apt-get update && apt-get install -yV tzdata

RUN apt-get update && apt-get install -yV \
    sudo \
    wget \
    curl \
    ssh \
    rsync \
    tar \
    unzip

RUN apt-get update && apt-get install -yV \
    # Coding
    python \
    vim \
    build-essential \
    cmake \
    gcc \
    g++ \
    gdb \
    clang \
    libboost-all-dev \
    libeigen3-dev \
    libgtk-3-dev \
    qt5-default \
    libvtk6-qt-dev \
    freeglut3-dev \
    libtbb-dev \
    libjpeg-dev \
    libopenjp2-7-dev \
    libpng++-dev \
    libtiff-dev \
    libopenexr-dev \
    libwebp-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswscale-dev \
    libavresample-dev \
    libhdf5-dev \
    # Graphics: SDL2, GLFW, GLM, GLEW
    libsdl2-dev \
    libx11-dev xorg-dev libglu1-mesa libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev \
    libglfw3 libglfw3-dev \
    libglm-dev \
    libglew-dev

# SSH
RUN apt-get update && apt-get install -yV x11-apps openssh-server && \
    mkdir /var/run/sshd && \
    echo 'root:password' | chpasswd && \
    sed -i 's/#*PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication/PasswordAuthentication/g' /etc/ssh/sshd_config && \
    sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# SSH user for CLion 
RUN useradd -ms /bin/bash clion && \
    echo 'clion:clion' | chpasswd

# Set DISPLAY globally for GDB
COPY display.env /tmp
RUN echo $(cat /tmp/display.env) >> /etc/environment && \
    rm /tmp/display.env

# manual cmake download
RUN cd /opt && \
    wget https://github.com/Kitware/CMake/releases/download/v3.20.6/cmake-3.20.6.tar.gz && \
    tar zxvf cmake-3.20.6.tar.gz && \
    cd cmake-3.20.6/ && \
    ./bootstrap && \
    make -j$(($(nproc) -1)) && sudo make install -j$(($(nproc) -1))

# Intel MKL setup and install
RUN cd /tmp && \
    wget https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB && \
    apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB && \
    rm GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB && \
    wget https://apt.repos.intel.com/setup/intelproducts.list -O /etc/apt/sources.list.d/intelproducts.list
RUN apt-get update && apt-get install -yV intel-mkl-2020.0-088

# install opencv under /opt
COPY opencv_build.sh .
RUN chmod 755 opencv_build.sh
RUN ["/bin/bash", "-c", "./opencv_build.sh"]

# create workspace
RUN mkdir /work && \
    mkdir /work/islay && \
    chown -R clion:clion /work

WORKDIR /

CMD ["/usr/sbin/sshd", "-D"]
