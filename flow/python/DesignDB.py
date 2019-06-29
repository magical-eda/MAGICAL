##
# @file DesignDB.py
# @author Keren Zhu
# @date 06/27/2019
# @brief The design database for the magical flow.
#

import magicalFlow
import pyparsing as _p

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
        for n in nets:
            self.nets[n] = net(n, self)
            self.nets[n].is_io = True
        
        self.instances = instances;
        for i in self.instances:
            # register subcircuit as parrent
            i.parent = self
            # add all internal nets
            for n in i.pins:
                if n not in self.nets:
                    self.nets[n] = net(n, self)
        
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

            self.translate_raw_netlist(netlist.parseString(nl))

    def translate_raw_netlist(self, raw):
        """
        @brief translate the raw netlist into DesignDB status
        @param the return value of the raw netlist
        """

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

