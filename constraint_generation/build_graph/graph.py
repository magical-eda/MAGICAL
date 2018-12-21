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

nmos_set = {"nmos", "nch", "nch_mac", "nch_lvt", "nch_lvt_mac", "nch_25_mac", "nch_na25_mac"}
pmos_set = {"pmos", "pch", "pch_mac", "pch_lvt", "pch_lvt_mac", "pch_25_mac"}
capacitor_set = {"crtmom", "crtmom_2t", "cfmom", "cfmom_2t"}
resistor_set = {"rppoly", "rppolyl", "rppolywo_m"}

power_net = {"vdd", "Vdd", "VDD", "vdda", "VDDA"}
ground_net = {"vss", "Vss", "VSS", "vssa", "VSSA", "gnd", "GND", "Gnd"}

def connected_mosfet(G, net_node):
    source_device = []
    drain_device = []
    gate_device = []
    connected_device = G.neighbors(net_node)
    for device in connected_device:
        if G.node[device]['nodetype'] in ["pmos","nmos"]:
            if 's' in G[net_node][device]['pin']:
                source_device.append(device)
            if 'd' in G[net_node][device]['pin']:
                drain_device.append(device)
            if 'g' in G[net_node][device]['pin']:
                gate_device.append(device)
    return (drain_device, gate_device, source_device)

def diff_pair(G):
    for node in G.nodes:
        if G.node[node]['nodetype'] == "net" and ( not G.node[node]['name'] in (power_net | ground_net) ):
            drain_nets = []
            drain_device, gate_device, source_device = connected_mosfet(G, node)
            if gate_device or (not drain_device) or (len(source_device) != 2):
                continue
            if G.node[source_device[0]]['connection'] == "any" and G.node[source_device[1]]['connection'] == "any":
                print("diff pair: ")
                print(G.node[source_device[0]]['name'])
                print(G.node[source_device[1]]['name'])
            #FIX: Assume only 2 source connect device. Could have use CMOS multiplier etc...

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
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="nmos", connection="any")
                print "added device node: ", parent+inst.name
            elif inst.reference in pmos_set:
                G.add_node(device_node_idx, name=parent+inst.name, nodetype="pmos", connection="any")
                print "added device node: ", parent+inst.name
            G.add_edge(lnets[0], device_node_idx, pin=['d']);
            if G.has_edge(lnets[1], device_node_idx):
                G.node[device_node_idx]['connection'] = 'diode'
                G[lnets[1]][device_node_idx]['pin'].append('g')
            else:
                G.add_edge(lnets[1], device_node_idx, pin='g')
            if G.has_edge(lnets[2], device_node_idx):
                if 'g' in G[lnets[2]][device_node_idx]['pin']:
                    G.node[device_node_idx]['connection'] = "dummy"
                if 'd' in G[lnets[2]][device_node_idx]['pin']:
                    G.node[device_node_idx]['connection'] = "cap"
                G[lnets[2]][device_node_idx]['pin'].append('s')
            else:
                G.add_edge(lnets[2], device_node_idx, pin='s')
            print "added edge: ", lnets[0], parent+inst.name+".d"
            print "added edge: ", lnets[1], parent+inst.name+".g"
            print "added edge: ", lnets[2], parent+inst.name+".s"
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
            for i in range(len(inst.pins)):
                if G.has_edge(lnets[i], device_node_idx):
                    G[lnets[i]][device_node_idx]['pin'].append(str(i))
                else:
                    G.add_edge(lnets[i], device_node_idx, pin=[str(i)])
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
    nx.draw(G)
    plt.show()
    
    diff_pair(G)

if __name__ == '__main__':
    main(sys.argv[1:])
