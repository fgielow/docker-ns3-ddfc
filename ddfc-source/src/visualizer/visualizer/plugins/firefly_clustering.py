import gtk

import sys

import ns.core
import ns.network
import ns.firefly_dynamic_clustering
import inspect

from visualizer.base import InformationWindow

class FireflyPyVizHelper(object):
    
    def __init__(self, dummyviz):
        self.fireflies = {}
        self.nodes = {}
        self.numberOfFireflies = 0
        self.edges=[]
        self.fireflyStatic = None
        
    def scan_nodes(self, viz):
        print "FIREF"
        print self
        print viz
        print "END FIREF"
        
        self.fireflyStatic = ns.firefly_dynamic_clustering.FireflyClustering
        
        for node in viz.nodes.itervalues():
            ns3_node = ns.network.NodeList.GetNode(node.node_index)
            fdc = None
            for i in range(ns3_node.GetNApplications()):
                app = ns3_node.GetApplication(i)
                if app.GetTypeId() == self.fireflyStatic.GetTypeId():
                    if (callable(app.getColor) and callable(app.getLabel)):
                        fdc = app
                
            if fdc is None:
                print "No FireflyClustering"
            else:
                self.fireflies[self.numberOfFireflies] = fdc
                self.nodes[self.numberOfFireflies] = node
                self.numberOfFireflies = self.numberOfFireflies + 1
        
        for i in range(self.numberOfFireflies):
            a=[]
            for j in range(self.numberOfFireflies):
                a.append(None)
            self.edges.append(a)
        
    def simulation_periodic_update(self, viz):
        for i in range(self.numberOfFireflies):
            self.nodes[i].set_color(self.fireflies[i].getColor())
            self.nodes[i].set_label(self.fireflies[i].getLabel())
#            self.nodes[i].set_label('('+str(i)+") "+str(self.fireflies[i].getReading()))
        
#        while (self.fireflyStatic.PyVizLocked()):
#            print "PY:: locked"
            
        while (self.fireflyStatic.getNewEdges_begin() >= 0):
            begin = self.fireflyStatic.getNewEdges_begin()
            end   = self.fireflyStatic.getNewEdges_end()

#            print "PY:: %d and %d" % (begin,end)
#            sys.stdout.flush()
            
            if self.edges[begin][end] is None:
                a = viz.create_edge(
                                    self.nodes[begin],
                                    self.nodes[end],
                                    self.fireflies[begin].getColor(),
                                    6.0
                                    )
                self.edges[begin][end] = a
                self.edges[end][begin] = a
                
        while (self.fireflyStatic.getEdgesToBeRemoved_begin() >= 0):
            begin = self.fireflyStatic.getEdgesToBeRemoved_begin()
            end   = self.fireflyStatic.getEdgesToBeRemoved_end()
            if self.edges[begin][end] is not None:
                self.edges[begin][end].destroy()
                self.edges[begin][end] = None
                self.edges[end][begin] = None

def register(viz):
    fc = FireflyPyVizHelper(viz)
    viz.connect("topology-scanned", fc.scan_nodes)
    viz.connect("simulation-periodic-update", fc.simulation_periodic_update)
    
    
    