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
#import matplotlib.pyplot as plt

nmos_set = {"nmos", "nch", "nch_mac", "nch_lvt", "nch_lvt_mac", "nch_25_mac", "nch_na25_mac", "nch_hvt_mac", "nch_25ud18_mac"}
pmos_set = {"pmos", "pch", "pch_mac", "pch_lvt", "pch_lvt_mac", "pch_25_mac", "pch_na25_mac", "pch_hvt_mac", "nch_25ud18_mac"}
capacitor_set = {"crtmom", "crtmom_2t", "cfmom", "cfmom_2t"}
resistor_set = {"rppoly", "rppolyl", "rppolywo_m"}

current_net_count = 0;
current_instance_count = 0;
dnets = dict()
message = []

def read_netlist( nl, inst, parent, lnets):
    global current_net_count
    global current_instance_count
    global dnets
    # find the reference
    is_subckt_flag = False
    for subckt in nl:
        if subckt.typeof == "subcircuit" and subckt.name == inst.reference:
            is_subckt_flag = True
            for sub_inst in subckt.instances:
                sub_lnets = []
                for pin in sub_inst.pins:
                    if subckt.nets[pin].is_io:
                        for idx in range(len(subckt.io_nets)):
                            if subckt.io_nets[idx] == pin:
                                sub_lnets.append(lnets[idx])
                                found_pin = True
                                break
                    else:
                        if parent+inst.name+'/'+pin in dnets:
                            sub_lnets.append(dnets[parent+inst.name+'/'+pin]) 
                        else:
                            dnets[parent+inst.name+'/'+pin] = current_net_count
                            sub_lnets.append(current_net_count)
                            current_net_count+=1
                read_netlist( nl, sub_inst, parent+inst.name+"/", sub_lnets)
            break
    # if reference not found, is a device
    if not is_subckt_flag:
        multi = 1
        if 'm' in inst.parameters:
            multi = inst.parameters['m']
        if 'multi' in inst.parameters:
            multi = inst.parameters['multi']
        if inst.reference in (nmos_set | pmos_set):
            device_node_idx = current_instance_count
            current_instance_count+=1 
            if inst.reference in nmos_set:
                print "Inst\n", current_instance_count-1, '\n', "NMOS\n", parent+inst.name
                print get_value(inst.parameters['w']), get_value(inst.parameters['l'])
            elif inst.reference in pmos_set:
                print "Inst\n", current_instance_count-1, '\n', "PMOS\n", parent+inst.name
                print get_value(inst.parameters['w']), get_value(inst.parameters['l'])
            print lnets[0]#, '\n', dnets.keys()[dnets.values().index(lnets[0])]
            print lnets[1]#, '\n', dnets.keys()[dnets.values().index(lnets[1])] 
            print lnets[2]#, '\n', dnets.keys()[dnets.values().index(lnets[2])] 
            print lnets[3]#, '\n', dnets.keys()[dnets.values().index(lnets[3])] 
        else:
            device_node_idx = current_instance_count
            current_instance_count+=1;
            if inst.reference in capacitor_set:
                print "Inst\n", current_instance_count-1, '\n', "CAP\n", parent+inst.name
                print get_value(inst.parameters['w'])
            elif inst.reference in resistor_set:
                print "Inst\n", current_instance_count-1, '\n', "RES\n", parent+inst.name
                print get_value(inst.parameters['wr']), get_value(inst.parameters['lr'])
            else:
                print "Inst\n", current_instance_count-1, '\n', "OTHER\n", parent+inst.name
            # TODO: differentiate resistor pins and capacitor pins
            # TODO: differentiate std cells
            for i in range(len(inst.pins)):
                print lnets[i]#, '\n', dnets.keys()[dnets.values().index(lnets[i])] 

def get_value(string):
    x = string[-1]
    if x.isalpha():
        if x == 'u':
            return string[:-1] + 'e-6'
        if x == 'n':
            return string[:-1] + 'e-9'
        return string[:-1]
    return string

def create_init_obj(nl):
    global current_net_count
    global current_instance_count
    global dnets
    G = nx.Graph()
    for inst in nl:
        if inst.typeof == "instance":
            lnets = []
            for pin in inst.pins:
                if pin in dnets:
                    lnets.append(dnets[pin])
                else:
                    lnets.append(current_net_count);
                    dnets[pin] = current_net_count;
                    current_net_count+=1; 
            read_netlist( nl, inst, "", lnets)

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hf:", ["file_name="])
    except getopt.GetoptError:
        print "typeof: ", sub_inst.typeof
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
    # parsed_netlist = parse_netlist.parse_hspice(nl)
    parsed_netlist = parse_netlist.parse_spectre(nl)
    fin.close()

    # write adjacency list to data
    G = create_init_obj(parsed_netlist)
    valuelist = dnets.values()
    valuelist.sort()
    for value in valuelist:  
        print "NET\n", value, '\n', dnets.keys()[dnets.values().index(value)] 

if __name__ == '__main__':
    main(sys.argv[1:])
