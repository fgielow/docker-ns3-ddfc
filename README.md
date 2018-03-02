ns3-ddfc
==========



This environment was setup for DDFC - a routing protocol implemented in NS3. You can find further information about the protocol here: http://www.nr2.ufpr.br/~fernando/MSc/


To help you, two aliases were created already.
You can execute the DDFC protocol example simulation by running: 

    $ ./waf --run firefly_dynamic_clustering-example

on the folder /workspace/bake/source/ns-3.14.1; you can also simply use the alias:

    $ ddfc


You can also opt to run the protocol with PyViz to visualize its working by using: 

    $ ./waf --run firefly_dynamic_clustering-example --vis 
You can also simply use the alias:

    $ ddfc-vis

Note that for the visualizer to appear, X forwarding must be functional between the Docker container and your most machine. Also, you will need to be properly connected to a network for this to work.



Any modifications you make on your local machine in the folder:

     ./ddfc-source/src/firefly_dynamic_clustering

will reflect on the simulations conducted on this environment.

The nodes being considered here and their positions can be found on:

     - ddfc-source/src/firefly_dynamic_clustering/examples/mote_locs.xml
     - ddfc-source/src/firefly_dynamic_clustering/examples/full_data.xml

# DEPENDENCIES

You will need the command **xhost** to work.
On MacOSX you can solve this by install Xquartz either by downloading the package and installing manually or by installing with brew:

    $ brew cask install xquartz

On Linux distributions you can simply install:

    $ sudo apt-get install x11-xserver-utils


And, of course, you need **DOCKER**. You can follow the instructions from here: https://docs.docker.com/install/linux/docker-ce/ubuntu/#install-docker-ce

Which at the time of writing are, fox apt-get based linux distrubitions:

    $ sudo apt-get update

    $ sudo apt-get install \
      apt-transport-https \
      ca-certificates \
      curl \
      software-properties-common

If you use other operating system, please use the power of google.


# STARTING THE CONTAINER

With the dependencies met, simply run

    $ make

If you wish to compile the container image locally instead of fetch from Docker Hub, you can simply:

    $ make building
