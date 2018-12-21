from pyparsing import *
import pyparsing as _p
import netlist as nl

def parse_spectre(netlist_string):
    # newlines are part of the grammar, thus redifine the whitespaces without it
    ws = ' \t'
    _p.ParserElement.setDefaultWhitespaceChars(ws)

    # spectre netlist grammar definition
    EOL = _p.LineEnd().suppress() # end of line
    linebreak = _p.Suppress("\\" + _p.LineEnd()) # breaking a line with backslash newline
    identifier=_p.Word(_p.alphanums+'_!<>-') # a name for...
    number=_p.Word(_p.nums + ".") # a number
    net = identifier # a net
    nets = _p.Group(_p.OneOrMore(net('net') | linebreak)) # many nets
    cktname = identifier # name of a subcircuit
    cktname_end = _p.Keyword("ends").suppress()
    comment = _p.Suppress("//" + _p.SkipTo(_p.LineEnd()))
    expression = _p.Word(_p.alphanums+'._*+-/()')
    inst_param_key = identifier + _p.Suppress("=")
    inst_param_value = expression('expression')
    inst_parameter = _p.Group(inst_param_key('name') + inst_param_value('value')).setResultsName('key')
    parameters = _p.Group(_p.ZeroOrMore(inst_parameter | linebreak)).setResultsName('parameters')
    instref = identifier
    instname = identifier
    instance = _p.Group(instname('name') + _p.Suppress('(') + nets('nets') + _p.Suppress(')') + instref('reference') + parameters + EOL).setResultsName('instance')
    subcircuit_content = _p.Group(_p.ZeroOrMore(instance | EOL | comment)).setResultsName('subnetlist')
    subcircuit = _p.Group(
        # matches subckt <name> <nets> <newline>
        _p.Keyword("subckt").suppress() + cktname('name') + nets('nets') + EOL  
        # matches the content of the subcircuit
        + subcircuit_content
        # matches ends <name> <newline>
        + cktname_end + _p.matchPreviousExpr(cktname).suppress() + EOL).setResultsName('subcircuit')
    netlist_element = subcircuit | instance | EOL | comment('comment')
    netlist = _p.ZeroOrMore(netlist_element) + _p.StringEnd()
    
    parameters.setParseAction(handle_parameters)
    instance.setParseAction(handle_instance)
    subcircuit.setParseAction(handle_subcircuit)

    return netlist.parseString(netlist_string);

def parse_hspice(netlist_string):
    # newlines are part of the grammar, thus redifine the whitespaces without it
    ws = ' \t'
    _p.ParserElement.setDefaultWhitespaceChars(ws)

    # spectre netlist grammar definition
    EOL = _p.LineEnd().suppress() # end of line
    linebreak = _p.Suppress(_p.LineEnd() + "+") # breaking a line with backslash newline
    identifier=_p.Word(_p.alphanums+'_!<>#-') # a name for...
    number=_p.Word(_p.nums + ".") # a number
    net = identifier # a net
    nets = _p.Group(_p.OneOrMore(net('net') + ~_p.FollowedBy("=") | linebreak)) # many nets
    cktname = identifier # name of a subcircuit
    cktname_end = _p.CaselessLiteral(".ends").suppress()
    comment = _p.Suppress("//" + _p.SkipTo(_p.LineEnd())) | _p.Suppress("*" + _p.SkipTo(_p.LineEnd()))
    expression = _p.Word(_p.alphanums+'._*+-/()')
    inst_param_key = identifier + _p.Suppress("=")
    inst_param_value = expression('expression')
    inst_parameter = _p.Group(inst_param_key('name') + inst_param_value('value')).setResultsName('key')
    parameters = _p.Group(_p.ZeroOrMore(inst_parameter | linebreak)).setResultsName('parameters')
    instname = identifier
    instnets = _p.Group(_p.OneOrMore(net('net') + ~_p.FollowedBy("=") | linebreak))
    instance = _p.Group(instname('name') + instnets('instnets') + parameters + EOL).setResultsName('instance')
    subcircuit_content = _p.Group(_p.ZeroOrMore(instance | EOL | comment)).setResultsName('subnetlist')
    subcircuit = _p.Group(
        # matches subckt <name> <nets> <newline>
        _p.CaselessLiteral(".subckt").suppress() + cktname('name') + _p.Optional(nets('nets')) + EOL  
        # matches the content of the subcircuit
        + subcircuit_content
        # matches ends <name> <newline>
        + cktname_end + _p.matchPreviousExpr(cktname).suppress() + EOL).setResultsName('subcircuit')
    netlist_element = subcircuit | instance | EOL | comment('comment')
    netlist = _p.ZeroOrMore(netlist_element) + _p.StringEnd()
    
    parameters.setParseAction(handle_parameters)
    instance.setParseAction(handle_instance)
    subcircuit.setParseAction(handle_subcircuit)

    return netlist.parseString(netlist_string)

def handle_parameters(token):
    d = {}
    for p in token.parameters:
        d[p[0]] = p[1]
    return d

def handle_subcircuit(token):
    sc = token.subcircuit
    nets = sc.nets
    name = sc.name
    instances = sc.subnetlist
    s = nl.subcircuit(name, nets, instances)
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
    i = nl.instance(name, pins, reference, parameters)
    return [i]

