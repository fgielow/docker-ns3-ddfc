FROM ubuntu:12.04
MAINTAINER Fernando Gielow <f.gielow@gmail.com>
LABEL Description="Docker image for NS-3 Network Simulator with DDFC routing protocol"


RUN apt-get update

# BASE
RUN apt-get -y install \
curl sudo wget \
g++ gcc \
gdb valgrind \
gsl-bin libgsl0-dev libgsl0ldbl \
flex bison libfl-dev \
tcpdump \
sqlite sqlite3 libsqlite3-dev \
libxml2 libxml2-dev \
uncrustify \
doxygen graphviz imagemagick \
dia \
libboost-signals-dev libboost-filesystem-dev \
autoconf automake build-essential \
mercurial git cvs \
bzr cmake unzip \
checkinstall libpcap-dev \
qt4-qmake qt4-dev-tools

# PYTHON DEPENDENCIES
RUN apt-get -y install python python-dev python-pygoocanvas python-pygraphviz \
python-kiwi libgoocanvas-dev \
python-gnome2 \
python-rsvg \
python-pygccxml python-setuptools ipython


# NS3 INSTALLATION
RUN mkdir /workspace
WORKDIR /workspace

RUN hg clone http://code.nsnam.org/bake
WORKDIR /workspace/bake

# custom bakeconf and ns-3.14.1 packages because this version was not maintained on Mercury repo
# if you need newer versions just remove these two lines and use appropriate environment on bake -e parameter
COPY ./container-dependencies/bakeconf.xml /workspace/bake/bakeconf.xml

COPY ./container-dependencies/nsc-0.5.3.tar.bz2 /workspace/bake/nsc-0.5.3.tar.bz2
COPY ./container-dependencies/ns-3.14.1.tar.bz2 /workspace/bake/ns-3.14.1.tar.bz2
COPY ./container-dependencies/pybindgen-0.15.0.809.tar.bz2 /workspace/bake/pybindgen-0.15.0.809.tar.bz2
COPY ./container-dependencies/netanim-3.100.tar.bz2 /workspace/bake/netanim-3.100.tar.bz2

RUN ./bake.py configure -e ns-allinone-3.14.1
RUN ./bake.py check -vvv
RUN ./bake.py download -vvv
RUN ./bake.py build -vvv

# ROOM FOR OPTIMISATION if these are needed (both should be available since they are compiled - albeit for 3.17):
# NS-3 OpenFlow Integration     : not enabled (OpenFlow not enabled (see option --with-openflow))
# NS-3 Click Integration        : not enabled (nsclick not enabled (see option --with-nsclick))
# 


# needed to generate python bindings -- late discovery
RUN apt-get install -y g++-multilib

# dclock just for testing X redirection from docker to host
# RUN apt-get -y install vim dclock






# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!  AFTER THIS POINT, WE HAVE A FUNCTIONAL NS3.14.1 INSTALLATION, NOW IT'S TIME TO INSTALL DDFC   !!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



WORKDIR /workspace/bake/source/ns-3.14.1

COPY ./ddfc-source.tar.bz2 /workspace/bake/source/ns-3.14.1

RUN tar xvjf ./ddfc-source.tar.bz2 && rsync -avv ./ddfc-source/src/* ./src/

# remove files with name starting with . on plugins, that broke everything
RUN find ./src/visualizer/visualizer/plugins -name "\.*" -exec rm {} \;

RUN ./waf configure --build-profile debug --enable-examples --enable-tests && ./waf

RUN ln -s /workspace/bake/source/ns-3.14.1/src/firefly_dynamic_clustering/examples/mote_locs.xml /opt/
RUN ln -s /workspace/bake/source/ns-3.14.1/src/firefly_dynamic_clustering/examples/full_data.xml /opt/

RUN find /workspace/bake/source/ns-3.14.1/build/ -maxdepth 1 -name "*.so" -exec ln -s {} /usr/lib/ \;


# NOW BUILD PYTHON BINDINGS
RUN ./waf --apiscan=visualizer,firefly_dynamic_clustering


# helper script
COPY ./container-dependencies/ddfc-helper.sh /root/.ddfc-helper.sh
RUN echo '[[ -f /root/.ddfc-helper.sh ]] && source /root/.ddfc-helper.sh' >> /root/.bashrc


# CLEANUP
RUN apt-get clean && \
  rm -rf /var/lib/apt && \
  rm -r /workspace/bake/source/ns-allinone-3.14.1 && \
  rm -rf /workspace/bake/source/ns-3.14.1/ddfc-source && \
  rm -rf /workspace/bake/*.tar.bz2 /workspace/bake/source/*.tar.bz2 \
  rm -rf /workspace/bake/source/ns-3.14.1/ddfc-source.tar.bz2


