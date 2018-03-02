clear

echo "==========================================================================================="
echo ""
echo "This environment was setup for DDFC - a routing protocol implemented in NS3"
echo "The author of this protocol is Fernando Gielow <f.gielow@gmail.com> and was "
echo "developed with the help of the NR2 group -- http://www.nr2.ufpr.br/"
echo ""
echo "You can find further information about the protocol here:"
echo "http://www.nr2.ufpr.br/~fernando/MSc/"
echo ""

echo "To help you, two aliases were created already."
echo "You can execute the DDFC protocol example simulation by running: "
echo "    $ ./waf --run firefly_dynamic_clustering-example"
echo "on the folder /workspace/bake/source/ns-3.14.1; you can also simply use the alias:"
echo "    $ ddfc"
echo ""

echo "You can also opt to run the protocol with PyViz to visualize its working by using: "
echo "    $ ./waf --run firefly_dynamic_clustering-example --vis "
echo "You can also simply use the alias:"
echo "    $ ddfc-vis"
echo ""
echo "Note that for the visualizer to appear, X forwarding must be functional between the Docker container and your most machine."
echo "Also, you will need to be properly connected to a network for this to work."

echo ""
echo ""
echo "Any modifications you make on your local machine in the folder:"
echo "     ./ddfc-source/src/firefly_dynamic_clustering"
echo "will reflect on the simulations conducted on this environment."

echo ""
echo "The nodes being considered here and their positions can be found on:"
echo "     - ddfc-source/src/firefly_dynamic_clustering/examples/mote_locs.xml"
echo "     - ddfc-source/src/firefly_dynamic_clustering/examples/full_data.xml"


echo ""
echo "==========================================================================================="
echo ""
echo ""

alias ddfc-vis="cd /workspace/bake/source/ns-3.14.1; ./waf --run firefly_dynamic_clustering-example --vis"
alias ddfc="cd /workspace/bake/source/ns-3.14.1; ./waf --run firefly_dynamic_clustering-example"