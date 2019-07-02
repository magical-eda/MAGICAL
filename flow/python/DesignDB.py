##
# @file DesignDB.py
# @author Keren Zhu
# @date 06/27/2019
# @brief The design database for the magical flow.
#

import magicalFlow
import pyparsing as _p


nmos_set = {"nmos", "nch", "nch_na", "nch_mac", "nch_lvt", "nch_lvt_mac", "nch_25_mac", "nch_na25_mac", "nch_hvt_mac", "nch_25ud18_mac"}
pmos_set = {"pmos", "pch", "pch_mac", "pch_lvt", "pch_lvt_mac", "pch_25_mac", "pch_na25_mac", "pch_hvt_mac", "nch_25ud18_mac"}
capacitor_set = {"cfmom", "cfmom_2t"}
resistor_set = {"rppoly", "rppoly_m", "rppolywo_m", "rppolywo"}
unsupported_set = {"rppolyl", "crtmom", "crtmom_2t"}

class DesignDB(object):
    def __init__(self):
        self.db = magicalFlow.DesignDB()

    def read_spectre_netlist(self, sp_netlist):
        """
        @brief parse spectre netlist
        """

class netlist_element(object):
    def __init__(self, typeof):
        self.typeof = typeof
        self.parent = False
    def __str__(self):
        return(self.typeof)


class subcircuit(netlist_element):
    def __init__(self, name, nets, instances):
        self.name = name;
        self.labels = {}
        self.power_nets = []
        self.ground_nets = []
        self.internal_nets = []
        # dictionarry of net names,
        # key is net name, value is net object
        # marke these nets also as io
        self.io_nets = nets # MY NOTE: get the pin list 
        self.nets = {} #= nets;
        self.net_name = [] # [idx of net] = net name 
        net_idx = 0
        for n in nets:
            self.nets[n] = net(n, self)
            self.nets[n].is_io = True
            self.nets[n].idx = net_idx # Force the indices of nets follow the same io nets order in spice defination
            self.net_name.append(n)
            net_idx += 1
        
        self.instances = instances;
        for i in self.instances:
            # register subcircuit as parrent
            i.parent = self
            # add all internal nets
            for n in i.pins:
                if n not in self.nets:
                    self.nets[n] = net(n, self)
                    self.nets[n].idx = net_idx
                    self.net_name.append(n)
                    net_idx += 1

        netlist_element.__init__(self,'subcircuit')

    def __str__(self):
        insts = {}
        for i in self.instances:
            insts[i.name] = i.reference
        return(self.typeof + " " + self.name + "(" + str(self.nets) + "):" + str(insts))

    def map_instances(self, mapping_function):
        for i in range(len(self.instances)):
            self.instances[i] = mapping_function(self.instances[i])
            
    def map_nets(self, mapping_function):
        for n in self.nets:
            self.nets[n] = mapping_function(self.nets[n])
    
    def __repr__(self):
        return self.name

class instance(netlist_element):
    def __init__(self, name, pins, reference, parameters):
        self.name = name;
        self.pins = pins;
        self.reference = reference;
        self.parameters = parameters;
        netlist_element.__init__(self,'instance')
    def __str__(self):
        return(self.typeof + " " + self.name + "@" + self.reference + str(self.parameters))
    def get_attr(self):
        self.attr = self.reference.split('_')

class net(netlist_element):
    def __init__(self, name, parent):
        self.name = name
        self.nettype = 'standard'
        self.nodes = set()
        self.labels = {}
        self.is_vdd = False
        self.is_gnd = False
        self.is_internal = False
        self.is_io = False
        self.parent = parent
        self.idx = -1

    def connect(self, pin):
        self.nodes.add(pin)

    def __repr__(self):
        return (self.parent.__repr__() + "." + self.name)

class pin(netlist_element):
    def __init__(self, name, parent):
        self.name = name
        self.parent = parent
        self.net = False
        self.direction = False

    def connect(self,net):
        if not self.net:
            # get the net object from the subcircuit
            self.net = self.parent.parent.nets[net]
            self.net.connect(self)
    
    def __repr__(self):
        return (self.parent.__repr__() + "." + self.name)
         
class mosfet(instance):
    def __init__(self,instance):
        self.name = instance.name
        self.parent = instance.parent
        self.parameters = instance.parameters
        self.reference = instance.reference
        self.drain = pin('d',self)
        self.gate = pin('g',self)
        self.source = pin('s',self)
        self.bulk = pin('b',self)
        self.labels = {}

    def connect(self, drain, gate, source, bulk):
        self.drain.connect(drain)
        self.gate.connect(gate)
        self.source.connect(source)
        self.bulk.connect(bulk)

    def __str__(self):
        return(self.typeof + ":" + self.reference + '[' + self.drain.net.name + ', '+ self.gate.net.name + ', '+ self.source.net.name + ', '+ self.bulk.net.name + ']')

    def __repr__(self):
        return(self.parent.__repr__() + "." + self.name)
class nmos(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nmos')
        mosfet.__init__(self, instance)
class pmos(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pmos')
        mosfet.__init__(self, instance)
class nch(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nch')
        mosfet.__init__(self, instance)
class pch(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pch')
        mosfet.__init__(self, instance)
class nch_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nch_mac')
        mosfet.__init__(self, instance)
class pch_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pch_mac')
        mosfet.__init__(self, instance)
class nch_lvt(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nch_lvt')
        mosfet.__init__(self, instance)
class pch_lvt(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pch_lvt')
        mosfet.__init__(self, instance)
class nch_lvt_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nch_lvt_mac')
        mosfet.__init__(self, instance)
class pch_lvt_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pch_lvt_mac')
        mosfet.__init__(self, instance)
class nch_25_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nch_25_mac')
        mosfet.__init__(self, instance)
class pch_25_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pch_25_mac')
        mosfet.__init__(self, instance)
class nch_na25_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'nch_na25_mac')
        mosfet.__init__(self, instance)
class pch_na25_mac(mosfet):
    def __init__(self,instance):
        netlist_element.__init__(self,'pch_na25_mac')
        mosfet.__init__(self, instance)

class Netlist_parser(object):
    def __init__(self, mdb):
        """
        @brief the constructor of Netlist_parser
        @param a magicalFlow.DesignDB()
        """
        self.db = mdb
        self._finish_raw_parse = False # Flag for whether the first step of parsing the raw netlist has been finished
    def parse_spectre(self, netlist_file):
        """
        @brief parse the input spectre netlist file
        @param .sp file
        """
        with open(netlist_file, 'r') as fin:
            nl = fin.read()
            ws = ' \t' #newlines are part of the syntax, thus redefine the whitespaces without it
            _p.ParserElement.setDefaultWhitespaceChars(ws)

            # spectre netlist syntax definition
            EOL = _p.LineEnd().suppress() #end of line
            linebreak = _p.Suppress("\\" + _p.LineEnd()) #breaking a line with backslash newline
            identifier  = _p.Word(_p.alphanums+'_!<>-+') # A name for ...
            number = _p.Word(_p.nums + ".") # a number
            net = identifier # a net
            nets = _p.Group(_p.OneOrMore(net('net') | linebreak)) #many nets
            cktname = identifier #name of a subckrcuit
            cktname_end = _p.Keyword("ends").suppress()
            comment = _p.Suppress("//" + _p.SkipTo(_p.LineEnd()))
            expression = _p.Word(_p.alphanums+'._*+-/()')
            inst_param_key = identifier + _p.Suppress('=')
            inst_param_value = expression("expression")
            inst_parameter = _p.Group(
                                      inst_param_key('name') 
                                      + inst_param_value('value')
                                      ).setResultsName('key')
            parameters = _p.Group(_p.ZeroOrMore(inst_parameter | linebreak)).setResultsName('parameters')
            instref = identifier
            instname = identifier
            instance = _p.Group( 
                                instname('name') 
                                + _p.Suppress('(') 
                                + nets('nets') 
                                + _p.Suppress(')') 
                                + instref('reference') 
                                + parameters 
                                + EOL
                                ).setResultsName('instance')
            subcircuit_content = _p.Group(_p.ZeroOrMore(instance | EOL | comment)).setResultsName('subnetlist')
            subcircuit = _p.Group(
                                  # matches subckt <name> <nets> <newline>
                                  _p.Keyword('subckt').suppress()
                                  + cktname('name')
                                  + nets('nets')
                                  + EOL
                                  # matches the content of the subcircuit
                                  + subcircuit_content
                                  # matches ends <name> <newline>
                                  + cktname_end
                                  + _p.matchPreviousExpr(cktname).suppress()
                                  + EOL
                                  ).setResultsName('subcircuit')
            topcircuit = _p.Group(
                                  # matches topckt <name> <nets> <newline>
                                  _p.Keyword('topckt').suppress()
                                  + cktname('name')
                                  + nets('nets')
                                  + EOL
                                  # matches the content of the subcircuit
                                  + subcircuit_content
                                  # matches ends <name> <newline>
                                  + cktname_end
                                  + _p.matchPreviousExpr(cktname).suppress()
                                  + EOL
                                  ).setResultsName('topcircuit')
            netlist_element = subcircuit | topcircuit | EOL | comment('comment')
            netlist = _p.ZeroOrMore(netlist_element) + _p.StringEnd()

            parameters.setParseAction(handle_parameters)
            instance.setParseAction(handle_instance)
            subcircuit.setParseAction(handle_subcircuit)
            topcircuit.setParseAction(handle_topcircuit)

            self.raw_netlist = netlist.parseString(nl) # Parse the file into raw_netlist and then translate into database

            self._finish_raw_parse = True
            self.translate_raw_netlist()

    def translate_raw_netlist(self):
        """
        @brief translate the raw netlist into DesignDB status
        """
        assert(self._finish_raw_parse)
        self.inst_name_map = {} # self.instance_name_map["name"] = index in self.ckt_list
        self.inst_pin_maps = [] # inst_pin_maps[ckt index][io pin index] = index of net index in the db ckt
        self.ckt_list = [] # Store the individual circuit and leaf instance in this list 
        found_top = False # whether the top circuit is found in the netlist
        topckt_idx = -1 # the index of top circuit in the self.ckt_list
        for ckt in self.raw_netlist:
            if ckt.typeof == "topcircuit":
                assert(not found_top) # No two top in the netlist
                self.translate_ckt(ckt)
                topckt_idx = len(self.ckt_list)
                found_top = True
            elif ckt.typeof == "subcircuit":
                self.translate_ckt(ckt)
            else:
                print("Unexpected circuit types ", ckt.typeof)
                assert(False)

        # Connected the constructed nodes to subgraphs
        ckt_idx = 0
        for ckt in self.raw_netlist:
            if ckt.typeof == "topcircuit":
                self.connect_children(ckt, ckt_idx)
                ckt_idx += 1
            elif ckt.typeof == "subcircuit":
                self.connect_children(ckt, ckt_idx)
                ckt_idx += 1
            else:
                print("Unexpected circuit types ", ckt.typeof)
                assert(False)

    def connect_children(self, ckt, ckt_idx):
        """
        @brief connect the translated circuits to their children in db
        @param ckt: a topcircuit or subcircuit object
        @param ckt_idx: the index of ckt
        """
        pin_idx = 0 # Pin index should be follow [inst][inst.net].flatten
        for node_idx in range(len(ckt.instances)):
            inst = ckt.instances[node_idx]
            subckt_idx = -1 # the circuit index of this instance. -1 if leaf
            if inst.reference in self.inst_name_map: # The instance is a subcircuit
                subckt_idx = self.inst_name_map[inst.reference]
            if subckt_idx != -1: # sub circuit
                self.db.subCkt(ckt_idx).node(node_idx).graphIdx = subckt_idx # The node (inst in ckt) is not leaf and the subgraph is the one found
                for inst_pin_idx in range(len(inst.pins)):
                    sub_net_idx = self.inst_pin_maps[subckt_idx][inst_pin_idx] # The net index connect to the pin in the sub circuit of "ckt"
                    self.db.subCkt(ckt_idx).pin(pin_idx).intNetIdx = sub_net_idx # The pin of instance is connected to one net in the sub circuit
                    pin_idx += 1
            else: # leaf
                subckt_idx = self.db.allocateCkt()
                self.db.subCkt(subckt_idx).name = inst.name
                for net_name in inst.pins:
                    sub_net_idx = self.db.subCkt(subckt_idx).allocateNet()
                    self.db.subCkt(subckt_idx).net(sub_net_idx).name = net_name
                    sub_pin_idx = self.db.subCkt(subckt_idx).allocatePin()
                    sub_node_idx = self.db.subCkt(subckt_idx).allocateNode()
                    self.db.subCkt(subckt_idx).pin(sub_pin_idx).nodeIdx = sub_node_idx
                    self.db.subCkt(subckt_idx).pin(sub_pin_idx).netIdx = sub_net_idx
                    self.db.subCkt(subckt_idx).net(sub_net_idx).appendPinIdx(sub_pin_idx)
                    self.db.subCkt(subckt_idx).node(sub_node_idx).appendPinIdx(sub_pin_idx)
                    self.db.subCkt(subckt_idx).node(sub_node_idx).refName = inst.reference
                    self.db.subCkt(subckt_idx).node(sub_node_idx).name = inst.name
                    self.db.subCkt(ckt_idx).pin(pin_idx).intNetIdx = sub_net_idx # from ckt to subckt
                    pin_idx += 1

    def translate_ckt(self, ckt):
        """
        @brief translate a top/sub circuit from raw_netlist
        @param a circuit want to parse
        """
        assert(self._finish_raw_parse)
        # Add the circuit to the self.ckt_list and self.instance_name_map
        self.ckt_list.append(ckt)
        self.inst_name_map[ckt.name] = len(self.ckt_list) - 1
        inst_pin_map = []

        # Allocate a new circuit in the cpp database
        # The index in the self.ckt_list and cpp database should be identical
        ckt_idx = self.db.allocateCkt()
        self.db.subCkt(ckt_idx).name = ckt.name
        assert(ckt_idx == len(self.ckt_list) - 1)

        for net_idx in range(len(ckt.nets)):
            db_net_idx = self.db.subCkt(ckt_idx).allocateNet()
            self.db.subCkt(ckt_idx).net(db_net_idx).name = ckt.net_name[net_idx]
            assert(db_net_idx == net_idx)
            if ckt.nets[ckt.net_name[net_idx]].is_io:
                inst_pin_map.append(net_idx) # [index of io_net/pin in ckt] = index of net in db 

        for inst_idx in range(len(ckt.instances)):
            inst = ckt.instances[inst_idx]
            db_node_idx = self.db.subCkt(ckt_idx).allocateNode()
            self.db.subCkt(ckt_idx).node(db_node_idx).refName = inst.reference
            self.db.subCkt(ckt_idx).node(db_node_idx).name = inst.name
            assert(db_node_idx == inst_idx)
            for net_name in inst.pins: # the same "pin" in inst.pins is the index of net it connecting to
                net_idx = ckt.nets[net_name].idx
                db_pin_idx = self.db.subCkt(ckt_idx).allocatePin()
                self.db.subCkt(ckt_idx).pin(db_pin_idx).nodeIdx = db_node_idx
                self.db.subCkt(ckt_idx).pin(db_pin_idx).netIdx = net_idx # the intNetIdx need to be set elsewhere
                self.db.subCkt(ckt_idx).net(net_idx).appendPinIdx(db_pin_idx)
                self.db.subCkt(ckt_idx).node(db_node_idx).appendPinIdx(db_pin_idx) # The subgraphIdx need to be set elsewhere

        self.inst_pin_maps.append(inst_pin_map)


def handle_parameters(token):
    d = {}
    for p in token.parameters:
        d[p[0]] = p[1]
    return d

def handle_topcircuit(token):
    sc = token.topcircuit
    nets = sc.nets
    name = sc.name
    instances = sc.subnetlist
    s = subcircuit(name, nets, instances)
    s.typeof = 'topcircuit'
    return [s]

def handle_subcircuit(token):
    sc = token.subcircuit
    nets = sc.nets
    name = sc.name
    instances = sc.subnetlist
    s = subcircuit(name, nets, instances)
    return [s]

def handle_instance(token):
    inst = token.instance
    name = inst.name
    if inst.nets: # this is the case for spectre
        pins = inst.nets
        reference = inst.reference
    elif inst.instnets: # this is the case for hspice
        pins = inst.instnets[0:-1]
        reference = inst.instnets[-1]
    parameters = inst.parameters
    i = instance(name, pins, reference, parameters)
    return [i]

