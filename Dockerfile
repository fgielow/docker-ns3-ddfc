FROM ubuntu:12.04
MAINTAINER Fernando Gielow <f.gielow@gmail.com>
LABEL Description="Docker image for NS-3 Network Simulator with DDFC routing protocol"

# RUN apt-get update && apt-get install -y curl sudo wget aria2

# install apt-fast for parallel apt-get install
# RUN wget https://raw.githubusercontent.com/ilikenwf/apt-fast/master/apt-fast -O /usr/local/sbin/apt-fast && \
# chmod +x /usr/local/sbin/apt-fast && \
# wget https://raw.githubusercontent.com/ilikenwf/apt-fast/master/apt-fast.conf -O /etc/apt-fast.conf


RUN apt-get update

RUN apt-get -y install \
curl sudo wget \
g++ gcc \
# g++-3.4 gcc-3.4 \
# g++-4.6 gcc-4.6 \
gdb valgrind \
# libgsl2
gsl-bin libgsl0-dev libgsl0ldbl \
flex bison libfl-dev \
tcpdump \
sqlite sqlite3 libsqlite3-dev \
libxml2 libxml2-dev \
#libgtk2.0-0 libgtk2.0-dev \
uncrustify \
doxygen graphviz imagemagick \
dia \
libboost-signals-dev libboost-filesystem-dev \
# gcc-multilib \
# gccxml \
autoconf automake build-essential \
mercurial git cvs \
# unrar
bzr cmake unzip \
checkinstall libpcap-dev \
qt4-qmake qt4-dev-tools
# boost-devel

RUN apt-get -y install python python-dev python-pygoocanvas python-pygraphviz \
python-kiwi libgoocanvas-dev \
python-gnome2 \
python-rsvg \
python-pygccxml python-setuptools ipython


#ns3
run mkdir /workspace
WORKDIR /workspace

RUN hg clone http://code.nsnam.org/bake
WORKDIR /workspace/bake

# custom bakeconf and ns-3.14.1 packages because this version was not maintained on Mercury repo
# if you need newer versions just remove these two lines and use appropriate environment on bake -e parameter
COPY ./bakeconf.xml /workspace/bake/bakeconf.xml

COPY ./nsc-0.5.3.tar.bz2 /workspace/bake/nsc-0.5.3.tar.bz2
COPY ./ns-3.14.1.tar.bz2 /workspace/bake/ns-3.14.1.tar.bz2
COPY ./pybindgen-0.15.0.809.tar.bz2 /workspace/bake/pybindgen-0.15.0.809.tar.bz2
COPY ./netanim-3.100.tar.bz2 /workspace/bake/netanim-3.100.tar.bz2

RUN ./bake.py configure -e ns-allinone-3.14.1
RUN ./bake.py check -vvv
RUN ./bake.py download -vvv
RUN ./bake.py build -vvv

# ROOM FOR OPTIMISATION if these are needed (both should be available since they are compiled - albeit for 3.17):
# NS-3 OpenFlow Integration     : not enabled (OpenFlow not enabled (see option --with-openflow))
# NS-3 Click Integration        : not enabled (nsclick not enabled (see option --with-nsclick))
# 

WORKDIR /workspace/bake/source/ns-3.14.1

COPY ./ns-3-dev-ddfc.tar.bz2 /workspace/bake/source/ns-3.14.1

RUN tar xvjf ./ns-3-dev-ddfc.tar.bz2

RUN rsync -avv ./ns-3-dev/src/* ./src/

RUN ./waf configure --enable-examples --enable-tests && ./waf

# CLEANUP
RUN apt-get clean && \
  rm -rf /var/lib/apt && \
  rm -r /workspace/bake/source/ns-allinone-3.14.1 && \
  rm -rf /workspace/bake/*.tar.bz2 /workspace/bake/source/*.tar.bz2




