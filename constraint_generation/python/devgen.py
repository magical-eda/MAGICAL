# All units are in um
import sys, getopt, math
import gdspy
import device_generation.basic as basic
from device_generation.Mosfet import Mosfet
from device_generation.Resistor import Resistor
from device_generation.Capacitor import Capacitor
import spice_parse.create_init_obj as init

capacitor_set = {"cap", "cap_2t"}
resistor_set = {"res", "res_m"}

def print_pin(cell, outFile):
    offset = get_offset(cell)
    of = open(outFile, 'a')
    of.write(cell.name+'\n')
    for pin in cell.pin():
        pin.adjust(offset)
        of.write(str(pin))
    of.close()

def get_offset(cell):
    offset = basic.offset(cell)
    offset[0] = math.floor(offset[0]/0.01)*0.01
    offset[1] = math.floor(offset[1]/0.01)*0.01
    return offset

def print_offset(cell, outFile):
    of = open(outFile, 'a')
    offset = basic.offset(cell)
    offset[0] = math.floor(offset[0]/0.01)*0.01
    offset[1] = math.floor(offset[1]/0.01)*0.01
    of.write("%s %.2f %.2f\n" % (cell.name, offset[0], offset[1]))
    of.close()

def print_block(cell, outFile):
    of = open(outFile, 'a')
    block = basic.block(cell)
    of.write("%s %d %d\n" % (cell.name, block[0], block[1]))
    of.close()

def print_all(cell, topcell):
    print_pin(cell, topcell+'.pin')
    print_block(cell, topcell+'.block')
    print_offset(cell, topcell+'.offset')

def get_val(string):
    x = string[-1]
    temp = string
    if x.isalpha():
        if x == 'u':
            temp = temp[:-1] + 'e-6'
        if x == 'n':
            temp = temp[:-1] + 'e-9'
    temp = float(temp) * 1e6
    return temp

def main(argv):
    fname = argv[0] 
    path = argv[1]
    # read a netlist file
    fin = open(fname, 'r')
    nl = fin.read()
    # parse the netlist
    #parsed_netlist = init.parse_netlist.parse_hspice(nl)
    parsed_netlist = init.parse_netlist.parse_spectre(nl)
    fin.close()

    # get instance and output .InitData
    inst_set = init.create_init_obj(parsed_netlist, path)
    top_cell = path+inst_set[0].name
    open('%s.pin' % top_cell, 'w').close
    open('%s.offset' % top_cell, 'w').close
    open('%s.block' % top_cell, 'w').close

    # generate instance
    for inst in inst_set[1:]:
        # Generate Device
        if inst.attr[0] == 'nfet':
            cell = Mosfet(True, inst.name, get_val(inst.parameters['w']), get_val(inst.parameters['l']), int(inst.parameters['nf']), inst.attr[1:])
        elif inst.attr[0] == 'pfet':
            cell = Mosfet(False, inst.name, get_val(inst.parameters['w']), get_val(inst.parameters['l']), int(inst.parameters['nf']), inst.attr[1:])
        elif inst.attr[0] == 'cap':
            cell = Capacitor(inst.name, get_val(inst.parameters['w']), get_val(inst.parameters['s']), int(inst.parameters['nr']), 
                get_val(inst.parameters['lr']), int(inst.parameters['stm']), int(inst.parameters['spm']), inst.attr[1:], get_val(inst.parameters['ftip']))
        elif inst.attr[0] == 'res':
            if 'series' in inst.parameters.keys():
                cell = Resistor(True, inst.name, get_val(inst.parameters['wr']), get_val(inst.parameters['lr']), int(inst.parameters['series']),
                    get_val(inst.parameters['segspace']), inst.attr[1:])
            elif 'para' in inst.parameters:
                cell = Resistor(False, inst.name, get_val(inst.parameters['wr']), get_val(inst.parameters['lr']), int(inst.parameters['para']),
                    get_val(inst.parameters['segspace']), inst.attr[1:])
            else:
                cell = Resistor(False, inst.name, get_val(inst.parameters['wr']), get_val(inst.parameters['lr']), 1,
                    0.20, inst.attr[1:])
        else:
            print >> sys.stderr, "instance not supported"
        gdspy.write_gds(path+'gds/'+inst.name+'.gds', [cell], unit=1.0e-6, precision=1.0e-9)
        print_all(cell, top_cell)

    print inst_set[0].name

if __name__ == '__main__':
    main(sys.argv[1:])
