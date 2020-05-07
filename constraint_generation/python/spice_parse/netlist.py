class netlist_element():
    def __init__(self,typeof):
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
