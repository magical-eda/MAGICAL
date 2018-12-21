#########################################################################
# File Name: build_graph.py
# Author: Biying Xu 
# mail: biying@utexas.edu
# Created Time: Mon 29 Oct 2018 05:38:35 PM CDT
# Modifided By: Mingjie Liu
#########################################################################
import sys, getopt
import parse_netlist
import networkx as nx
import matplotlib.pyplot as plt

nmos_set = {"nmos", "nch", "nch_mac", "nch_lvt", "nch_lvt_mac", "nch_25_mac", "nch_na25_mac", "nch_hvt_mac"}
pmos_set = {"pmos", "pch", "pch_mac", "pch_lvt", "pch_lvt_mac", "pch_25_mac", "pch_na25_mac", "pch_hvt_mac"}
capacitor_set = {"crtmom", "crtmom_2t", "cfmom", "cfmom_2t"}
resistor_set = {"rppoly", "rppolyl", "rppolywo_m"}

def construct_graph_inst(G, nl, inst, parent, lnets):
    # find the reference
    print "construct_graph_inst: ", inst.reference
    is_subckt_flag = False
    for subckt in nl:
        if subckt.typeof == "subcircuit" and subckt.name == inst.reference:
            is_subckt_flag = True
            print "reference in nl"
            for sub_inst in subckt.instances:
                print "name: ", sub_inst.name
                print "typeof: ", sub_inst.typeof
                sub_lnets = []
                for pin in sub_inst.pins:
                    if subckt.nets[pin].is_io:
                        for idx in range(len(subckt.io_nets)):
                            if subckt.io_nets[idx] == pin:
                                sub_lnets.append(lnets[idx])
                                break
                    else:
                        G.add_node(G.number_of_nodes(), name=parent+inst.name+"/"+pin, nodetype="net") 
                        print "added net node: ", parent+inst.name+"/"+pin
                        sub_lnets.append(G.number_of_nodes()-1)
                construct_graph_inst(G, nl, sub_inst, parent+inst.name+"/", sub_lnets)
            break
    # if reference not found, is a device
    if not is_subckt_flag:
        print "reference not in nl"
        if inst.reference in (nmos_set | pmos_set):
            device_node_idx = G.number_of_nodes()
            if inst.reference in nmos_set:
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="nmos")
                print "added device node: ", parent+inst.name
            elif inst.reference in pmos_set:
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="pmos")
                print "added device node: ", parent+inst.name
            G.add_node(device_node_idx+1, name=parent+inst.name+".d", nodetype="drain") # pins 
            G.add_node(device_node_idx+2, name=parent+inst.name+".g", nodetype="gate")
            G.add_node(device_node_idx+3, name=parent+inst.name+".s", nodetype="source")
            G.add_node(device_node_idx+4, name=parent+inst.name+".b", nodetype="bulk") # should we consider bulk?
            print "added pin nodes: ", parent+inst.name, ".d, .g, .s, .b"
            # add graph edges between device and pins
            G.add_edge(device_node_idx, device_node_idx+1);
            G.add_edge(device_node_idx, device_node_idx+2);
            G.add_edge(device_node_idx, device_node_idx+3);
            G.add_edge(device_node_idx, device_node_idx+4);
            print "added edges: ", parent+inst.name, parent+inst.name+".d, .g, .s, .b"
            # add graph edges between pins and nets
            # pin order is drain, gate, source, bulk
            G.add_edge(lnets[0], device_node_idx+1);
            G.add_edge(lnets[1], device_node_idx+2);
            G.add_edge(lnets[2], device_node_idx+3);
            G.add_edge(lnets[3], device_node_idx+4);
            print "added edge: ", lnets[0], parent+inst.name+".d"
            print "added edge: ", lnets[1], parent+inst.name+".g"
            print "added edge: ", lnets[2], parent+inst.name+".s"
            print "added edge: ", lnets[3], parent+inst.name+".b"
        else:
            device_node_idx = G.number_of_nodes()
            if inst.reference in capacitor_set:
                print inst.reference, " is a capacitor"
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="cap")
            elif inst.reference in resistor_set:
                print inst.reference, " is a resistor"
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="res")
            else:
                print inst.reference, " is a not nmos, pmos, capacitor, or resistor"
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="other_device")
            print "added device node: ", parent+inst.name
            # TODO: differentiate resistor pins and capacitor pins
            for i in range(len(inst.pins)):
                G.add_node(G.number_of_nodes(), name=parent+inst.name+"."+inst.pins[i], nodetype="other_pin")
                print "added pin node: ", parent+inst.name+"."+inst.pins[i]
                # add graph edges between device and pins
                G.add_edge(device_node_idx, G.number_of_nodes()-1)
                print "added edge: ", parent+inst.name, parent+inst.name+"."+inst.pins[i]
                # add graph edges between pins and nets
                G.add_edge(lnets[i], G.number_of_nodes()-1)
                print "added edge: ", lnets[i], parent+inst.name+"."+inst.pins[i]
        print "number_of_nodes: ", G.number_of_nodes()

def construct_graph(nl):
    G = nx.Graph()
    dnets = dict()
    for inst in nl:
        print "typeof: ", inst.typeof
        if inst.typeof == "instance":
            print inst
            # add nets
            lnets = []
            for pin in inst.pins:
                print pin
                if pin in dnets:
                    lnets.append(dnets[pin])
                else:
                    G.add_node(G.number_of_nodes(), name=pin, nodetype="net")
                    print "added net node: ", pin
                    lnets.append(G.number_of_nodes()-1)
                    dnets[pin] = G.number_of_nodes()-1
                    print "number_of_nodes: ", G.number_of_nodes()
            construct_graph_inst(G, nl, inst, "", lnets)
    print dnets
    print lnets
    return(G)

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hf:", ["file_name="])
    except getopt.GetoptError:
        print 'build_graph.py -f <file_name>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'build_graph.py -f <file_name>'
            sys.exit()
        elif opt in ("-f"):
            fname = arg

    # read a netlist file
    fin = open(fname, 'r')
    nl = fin.read()
    # parse the netlist
    parsed_netlist = parse_netlist.parse_hspice(nl)
    # parsed_netlist = parse_netlist.parse_spectre(nl)
    fin.close()

    # print the top level objects
    for obj in parsed_netlist:
        print(obj)

    # write adjacency list to data
    G = construct_graph(parsed_netlist)
    plt.subplot(121)
    nx.draw(G, with_labels=True, font_weight='bold') 
    plt.show()

if __name__ == '__main__':
    main(sys.argv[1:])
