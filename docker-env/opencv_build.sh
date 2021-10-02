#!/bin/bash

######################################
# INSTALL OPENCV ON UBUNTU OR DEBIAN #
######################################

# -------------------------------------------------------------------- |
#                       SCRIPT OPTIONS                                 |
# ---------------------------------------------------------------------|
OPENCV_VERSION='4.5.1'       # Version to be installed
OPENCV_CONTRIB='YES'          # Install OpenCV's extra modules (YES/NO)
# -------------------------------------------------------------------- |


# 1. KEEP UBUNTU OR DEBIAN UP TO DATE

sudo apt-get -y update
# sudo apt-get -y upgrade       # Uncomment to install new versions of packages currently installed
# sudo apt-get -y dist-upgrade  # Uncomment to handle changing dependencies with new vers. of pack.
# sudo apt-get -y autoremove    # Uncomment to remove packages that are now no longer needed

# 2. INSTALL DEPENDENCIES

# skip...

# 3. INSTALL THE LIBRARY
cd /opt

# download and unzip opencv
wget -nc https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip
unzip ${OPENCV_VERSION}.zip && rm ${OPENCV_VERSION}.zip
# mv opencv-${OPENCV_VERSION} OpenCV

# download and unzip opencv-contrib
wget -nc https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip
unzip ${OPENCV_VERSION}.zip && rm ${OPENCV_VERSION}.zip
# mv opencv_contrib-${OPENCV_VERSION} opencv_contrib
# mv opencv_contrib OpenCV

GENERATOR_NAME="Unix Makefiles"
cd opencv-${OPENCV_VERSION} && mkdir build && cd build

cmake \
  -G "${GENERATOR_NAME}" \
  -DCMAKE_BUILD_TYPE=RELEASE \
  -DWITH_QT=ON -DWITH_OPENGL=ON -DWITH_VTK=ON -DWITH_TBB=ON \
  -DMKL_ROOT_DIR=/opt/intel/mkl \
  -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-${OPENCV_VERSION}/modules ..
# -DCUDA_FAST_MATH=ON -DWITH_CUDA=ON -DWITH_CUDNN=ON -DWITH_CUBLAS=ON \

make -j$(($(nproc) -1))

cd /

# 4. EXECUTE SOME OPENCV EXAMPLES AND COMPILE A DEMONSTRATION

# To complete this step, please visit 'http://milq.github.io/install-opencv-ubuntu-debian'.

