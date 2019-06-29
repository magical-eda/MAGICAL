##
# @file Params.py
# @author Keren Zhu
# @date 06/27/2019
# @brief The parameters of the flow
#

import json
import math

class Params:
    """
    @brief Parameter class
    """
    def __init__(self):
        """
        @brief initialization
        """
        self.spectre_netlist = None # Input netlist file

    def printWelcome(self):
        """
        @brief print welcome message
        """
        content = """\
========================================================
    MAGICAL: Machine Generated Analog IC Layout            
        https://github.com/magical-eda/MAGICAL
========================================================"""
        print(content)

    def printHelp(self):
        """
        @brief print help message for JSON parameters
        """
        content = """\
                    JSON Parameters
========================================================
spectre_netlist [required for spectre netlist]    | input .sp file 
        """ % (self.spectre_netlist 
                )
        print(content)


    def toJson(self):
        """
        @brief convert to json  
        """
        data = dict()
        data['spectre_netlist'] = self.spectre_netlist
        return data 

    def fromJson(self, data):
        """
        @brief load form json 
        """
        if 'spectre_netlist' in data: self.spectre = data['spectre_netlist']

    def dump(self, filename):
        """
        @brief dump to json file 
        """
        with open(filename, 'w') as f:
            json.dump(self.toJson(), f)

    def load(self, filename):
        """
        @brief load from json file 
        """
        with open(filename, 'r') as f:
            self.fromJson(json.load(f))

    def __str__(self):
        """
        @brief string 
        """
        return str(self.toJson())

    def __repr__(self):
        """
        @brief print 
        """
        return self.__str__()
