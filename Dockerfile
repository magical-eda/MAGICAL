FROM gcc:6.3
LABEL maintainer="Mingjie Liu <jay_liu@utexas.edu>"

# Install cmake 3.16.3
ADD https://cmake.org/files/v3.16/cmake-3.16.3-Linux-x86_64.sh /cmake-3.16.3-Linux-x86_64.sh
RUN mkdir /opt/cmake \
        && sh /cmake-3.16.3-Linux-x86_64.sh --prefix=/opt/cmake --skip-license \
        && ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake \
        && cmake --version

# Update apt packages
RUN apt update
RUN apt upgrade -y

# Install boost flex
RUN apt-get install -y \
    build-essential zlib1g-dev libncurses5-dev libnss3-dev libssl-dev \
    libreadline-dev libffi-dev \
    libboost-all-dev flex csh vim \
    python3-openssl 
    
# Update openssl
RUN apt-get remove -y openssl
RUN wget https://www.openssl.org/source/openssl-1.1.1d.tar.gz \
        && tar -zxf openssl-1.1.1d.tar.gz && cd openssl-1.1.1d \
        && ./config && make && make install \
        && ln -s /usr/local/bin/openssl /usr/bin/openssl && ldconfig \
        && openssl version
        
# Install python 3.7
RUN wget https://www.python.org/ftp/python/3.7.5/Python-3.7.5.tgz
RUN tar -xf Python-3.7.5.tgz && cd Python-3.7.5 \
        && ./configure --enable-shared --with-openssl=/openssl-1.1.1d/ \
            --enable-optimizations --with-ssl-default-suites=openssl \
            CFLAGS="-I /openssl-1.1.1d/include" LDFLAGS="-L /openssl-1.1.1d/" \
        && make && make install 
RUN echo "alias python=python3" >> ~/.bashrc \
        && echo "alias pip=pip3" >> ~/.bashrc
ENV PATH=${PATH}:/usr/bin/python3.7
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/usr/lib:/usr/local/lib
RUN /bin/bash -c "source ~/.bashrc"

# Install lp_solve
RUN git clone https://github.com/kerrickstaley/lp_solve.git  \
        && cd lp_solve/lpsolve55 \
        && sh ccc \
        && cp /lp_solve/lpsolve55/bin/ux64/* /lp_solve
ENV LPSOLVE_DIR=/lp_solve

# Install lemon
RUN wget lemon.cs.elte.hu/pub/sources/lemon-1.3.1.tar.gz
RUN tar -xf lemon-1.3.1.tar.gz \
        && mkdir lemon-1.3.1/build && cd lemon-1.3.1/build \ 
        && cmake -DCMAKE_INSTALL_PREFIX=/lemon .. \
        && make && make install 
ENV LEMON_DIR=/lemon

# Install wnlib
RUN mkdir wnlib && cd wnlib \
        && wget http://www.willnaylor.com/wnlib.tar.gz \
        && gunzip wnlib.tar.gz && tar xvf wnlib.tar \ 
        && export PATH=${PATH}:/wnlib/acc \
        && make all
ENV WNLIB_DIR=/wnlib

# Install eigen
RUN wget https://gitlab.com/libeigen/eigen/-/archive/3.3.3/eigen-3.3.3.tar.gz \
        && tar -xf eigen-3.3.3.tar.gz 
ENV EIGEN_INC=/eigen-3.3.3

# Install pybind11
RUN git clone https://github.com/pybind/pybind11.git
ENV PYBIND11_DIR=/pybind11

# Install sparsehash
RUN wget https://github.com/sparsehash/sparsehash/archive/refs/tags/sparsehash-2.0.4.tar.gz 
RUN tar -xf sparsehash-2.0.4.tar.gz \
        && cd sparsehash-sparsehash-2.0.4 \
        && ./configure --prefix=/sparsehash \
        && make && make install
ENV SPARSE_HASH_DIR=/sparsehash

# Install bison 3.4
RUN wget http://ftp.gnu.org/gnu/bison/bison-3.4.tar.gz
RUN tar -xf bison-3.4.tar.gz \
        && cd bison-3.4 \
        && ./configure && make && make install \
        && ln -s /usr/local/bin/bison /usr/bin/bison \
        && bison --version

# Install limbo
RUN git clone https://github.com/limbo018/Limbo.git
RUN mkdir Limbo/build && cd Limbo/build \
        && cmake .. -DCMAKE_INSTALL_PREFIX=/limbo \
        && make && make install
ENV LIMBO_DIR=/limbo
ENV LIMBO_INC=/limbo/include

# Install python prerequisite
SHELL ["/bin/bash", "-c"]
RUN ln -s /usr/local/bin/pip3 usr/local/bin/pip
RUN pip install networkx matplotlib scipy numpy Cython pybind11
RUN git clone https://github.com/jayl940712/gdspy.git \
        && pip install gdspy/

# Upgrade boost
RUN apt-get --purge remove -y libboost-dev libboost-all-dev
RUN wget -O boost_1_62_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.gz/download \
        && tar xzvf boost_1_62_0.tar.gz && cd boost_1_62_0/ \
        && ./bootstrap.sh --prefix=/usr/ \
        && ./b2 && ./b2 install

# Install magical
RUN mkdir install && cd install && git clone https://github.com/magical-eda/MAGICAL.git \
        && cd MAGICAL && git checkout docker \
        && git submodule init && git submodule update \
        && ./build.sh
