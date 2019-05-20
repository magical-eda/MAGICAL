import numpy as np
import sys, getopt

def write_pin_locs(nets, inst_locs, inst_pins, outfile):
    print "net length: ", len(nets)
    print "inst_locs length: ", len(inst_locs)
    print "inst_pins length: ", len(inst_pins)
    uu2dbu = 1000
    with open(outfile, 'w') as f:
        num_nets = len(nets)
        f.write("num net "+str(num_nets)+"\n")
        for i in range(num_nets):
            net_name = nets[i][0]
            terms = nets[i][1]
            #num_terms = len(terms)
            #f.write(net_name+" "+str(i)+" "+str(num_terms)+" 1\n")
            pin_cnt = 0
            for term in terms:
                inst_name = term[0]
                term_name = term[1]
                # FIXME: ignore resistor for now due to rotation problem
                if term_name in inst_pins[inst_name]:
                    pin_shapes = inst_pins[inst_name][term_name]
                    pin_cnt += len(pin_shapes)
            f.write(net_name+" "+str(i)+" "+str(pin_cnt)+" 1\n")
            for term in terms:
                inst_name = term[0]
                term_name = term[1]
                inst_x = inst_locs[inst_name][0]
                inst_y = inst_locs[inst_name][1]
                # FIXME: ignore resistor for now due to rotation problem
                if term_name in inst_pins[inst_name]:
                    pin_shapes = inst_pins[inst_name][term_name]
                    for pin_shape in pin_shapes:
                        layer = pin_shape[0]
                        xl = str(int((inst_x+pin_shape[1])*uu2dbu))
                        yl = str(int((inst_y+pin_shape[2])*uu2dbu))
                        xh = str(int((inst_x+pin_shape[3])*uu2dbu))
                        yh = str(int((inst_y+pin_shape[4])*uu2dbu))
                        if layer == "PO":
                            write_str = "0 "
                        elif layer[0] == 'M':
                            write_str = layer[1] + " "
                        write_str += xl+" "+yl+" "
                        write_str += xh+" "+yl+" "
                        write_str += xh+" "+yh+" "
                        write_str += xl+" "+yh+"\n"
                        f.write(write_str)

def read_result_file(infile):
    inst_locs = dict()
    with open(infile, 'r') as f:
        for line in f:
            words = line.split()
            inst_name = words[0]
            x = float(words[1])
            y = float(words[2])
            inst_locs[inst_name] = (x, y)
    return inst_locs

# == older version == #
def read_pin_file_old(pfile):
    inst_pins = dict()
    with open(pfile, 'r') as f:
        for line in f:
            words = line.split()
            if len(words) == 1:
                inst_name = words[0]
            else:
                num_shapes = (len(words)-2) / 4
                pin_name = words[0]
                shape_rects = []
                # shapes will always be rectangles
                for i in range(num_shapes):
                    xl = float(words[i*4+2].strip("()"))
                    yl = float(words[i*4+3].strip("()"))
                    xh = float(words[i*4+4].strip("()"))
                    yh = float(words[i*4+5].strip("()"))
                    shape_rects.append((xl, yl, xh, yl))
                inst_pins[inst_name][pin_name] = shape_rects
    return inst_pins

# == new version == #
def read_pin_file(pfile):
    inst_pins = dict()
    with open(pfile, 'r') as f:
        for line in f:
            words = line.split()
            length = len(words)
            if length == 1:
                inst_name = words[0]
                inst_pins[inst_name] = dict()
            elif length == 2:
                pin_name = words[0]
                num_shapes = int(words[1])
            else:
                shape_rects = []
                # shapes will always be rectangles
                for i in range(num_shapes):
                    layer = words[i*5]
                    xl = float(words[i*5+1].strip("()"))
                    yl = float(words[i*5+2].strip("()"))
                    xh = float(words[i*5+3].strip("()"))
                    yh = float(words[i*5+4].strip("()"))
                    shape_rects.append((layer, xl, yl, xh, yh))
                inst_pins[inst_name][pin_name] = shape_rects
    return inst_pins

def read_connection_file(nfile):
    #nets = dict()
    nets = []
    with open(nfile, 'r') as f:
        for line in f:
            words = line.split()
            num_terms = (len(words)-1) / 2
            net_name = words[0]
            terms = []
            for i in range(num_terms):
                inst_name = words[i*2+1]
                term_name = words[i*2+2]
                # FIXME: ignore bulk term
                if term_name != "B":
                    terms.append((inst_name, term_name))
            nets.append([net_name, terms])
    return nets

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hf:p:n:o:", ["file_name=", "pin_file_name=","net_file_name","out_file_name"])
    except getopt.GetoptError:
        #print 'legalize.py -f <file_name> -a <add_path>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            #print 'legalize.py -f <file_name>'
            sys.exit()
        elif opt in ("-f", "--file_name"):
            fname = arg
        elif opt in ("-p", "--pin_file_name"):
            pfname = arg
        elif opt in ("-n", "--net_file_name"):
            nfname = arg
        elif opt in ("-o", "--out_file_name"):
            ofname = arg
    inst_locs = read_result_file(fname)
    inst_pins = read_pin_file(pfname)
    nets = read_connection_file(nfname) 
    # pin_locs = get_pins(inst_locs, inst_pins)
    write_pin_locs(nets, inst_locs, inst_pins, ofname)


if __name__ == '__main__':
    main(sys.argv[1:])

