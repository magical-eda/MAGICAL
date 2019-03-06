#########################################################################
# File Name: create_init_obj.py
# Author: anoynmous anoynmous
# mail: jay_anoynmous@anoynmous
# Created Time: Mon 29 Oct 2018 05:38:35 PM CDT
#########################################################################
import sys, getopt
import parse_netlist

nmos_set = {"nmos", "nch", "nch_na", "nch_mac", "nch_lvt", "nch_lvt_mac", "nch_25_mac", "nch_na25_mac", "nch_hvt_mac", "nch_25ud18_mac"}
pmos_set = {"pmos", "pch", "pch_mac", "pch_lvt", "pch_lvt_mac", "pch_25_mac", "pch_na25_mac", "pch_hvt_mac", "nch_25ud18_mac"}
capacitor_set = {"cfmom", "cfmom_2t"}
resistor_set = {"rppoly", "rppoly_m", "rppolywo_m", "rppolywo"}
unsupported_set = {"rppolyl", "crtmom", "crtmom_2t"}

current_net_count = 0;
current_instance_count = 0;
dnets = dict()
instance_set = []
net_con = []

def read_netlist( nl, inst, parent, lnets, of):
    global current_net_count
    global current_instance_count
    global dnets
    global instance_set
    global net_con
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
                read_netlist( nl, sub_inst, parent+inst.name+"/", sub_lnets, of)
            break
    # if reference not found, is a device
    if not is_subckt_flag:
        new_inst = inst
        new_inst.name = parent+inst.name
        new_inst.get_attr()
        instance_set.append(new_inst)
        multi = 1
        if 'm' in inst.parameters:
            multi = inst.parameters['m']
        if 'multi' in inst.parameters:
            multi = inst.parameters['multi']
        if inst.reference in (nmos_set | pmos_set):
            device_node_idx = current_instance_count
            current_instance_count+=1 
            if inst.reference in nmos_set:
                of.write("Inst\n%d \n%s\n%s\n" % (current_instance_count-1,"NMOS", parent+inst.name))
                of.write("%s %s\n" % (get_value(inst.parameters['w']), get_value(inst.parameters['l'])))
            elif inst.reference in pmos_set:
                of.write("Inst\n%d \n%s\n%s\n" % (current_instance_count-1, "PMOS", parent+inst.name))
                of.write("%s %s\n" % (get_value(inst.parameters['w']), get_value(inst.parameters['l'])))
            for i in range(4):
                of.write("%d\n" % lnets[i])
            net_con[lnets[0]].append([new_inst.name,'D'])
            net_con[lnets[1]].append([new_inst.name,'G'])
            net_con[lnets[2]].append([new_inst.name,'S'])
            net_con[lnets[3]].append([new_inst.name,'B'])
        else:
            device_node_idx = current_instance_count
            current_instance_count+=1;
            if inst.reference in capacitor_set:
                of.write("Inst\n%d \n%s\n%s\n" % (current_instance_count-1, "CAP", parent+inst.name))
                of.write("%s\n" % get_value(inst.parameters['w']))
            elif inst.reference in resistor_set:
                of.write("Inst\n%d \n%s\n%s\n" % (current_instance_count-1, "RES", parent+inst.name))
                of.write('%s %s\n' % (get_value(inst.parameters['wr']), get_value(inst.parameters['lr'])))
            else:
                of.write("Inst\n%d \n%s\n%s\n" % (current_instance_count-1, "OTHER", parent+inst.name))
            # TODO: differentiate resistor pins and capacitor pins
            # TODO: differentiate std cells
            for i in range(len(inst.pins)):
                of.write("%d\n" % lnets[i])
            net_con[lnets[0]].append([new_inst.name,'PLUS'])
            net_con[lnets[1]].append([new_inst.name,'MINUS'])
            if len(inst.pins) > 2:
                net_con[lnets[2]].append([new_inst.name,'BULK'])

def get_value(string):
    x = string[-1]
    if x.isalpha():
        if x == 'u':
            return string[:-1] + 'e-6'
        if x == 'n':
            return string[:-1] + 'e-9'
        return string[:-1]
    return string

def create_init_obj(nl, path):
    global current_net_count
    global current_instance_count
    global dnets
    global instance_set
    global net_con
    for ckt in nl:
        if ckt.typeof == "topcircuit":
            of = open(path+ckt.name+'.initObj', 'w')
            pinf = open(path+ckt.name+'.iopin', 'w')
            instance_set.append(ckt)
            for net in ckt.io_nets:
                pinf.write('%s\n' % net)
            for inst in ckt.instances:
                if inst.typeof == "instance":
                    lnets = []
                    for pin in inst.pins:
                        if pin in dnets:
                            lnets.append(dnets[pin])
                        else:
                            lnets.append(current_net_count);
                            dnets[pin] = current_net_count;
                            net_con.append([])
                            current_net_count+=1; 
                    read_netlist( nl, inst, "", lnets, of)
            pinf.close()
    valuelist = dnets.values()
    valuelist.sort()
    for value in valuelist:  
        of.write("NET\n%d \n%s\n" % (value, dnets.keys()[dnets.values().index(value)])) 
    of.close()
    of = open(path+ckt.name+'.connection', 'w')
    of2 = open(path+ckt.name+'.net', 'w')
    for i in range(len(net_con)):
        of.write('%s ' % dnets.keys()[dnets.values().index(i)])
        of2.write('%s ' % dnets.keys()[dnets.values().index(i)])
        for pin in net_con[i]:
            of.write('%s %s ' % (pin[0], pin[1]))
            of2.write('%s 0 0 ' % pin[0])
        of.write('\n')
        of2.write('\n')
    of.close()
    of2.close()
    return instance_set

def main(argv): 
    fname = argv[0]
    fout = argv[1]
    # read a netlist file
    fin = open(fname, 'r')
    nl = fin.read()
    # parse the netlist
    #parsed_netlist = parse_netlist.parse_hspice(nl)
    parsed_netlist = parse_netlist.parse_spectre(nl)
    fin.close()

    # write adjacency list to data
    create_init_obj(parsed_netlist, fout)

if __name__ == '__main__':
    main(sys.argv[1:])
