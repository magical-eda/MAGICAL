##
# @file Magical.py
# @author Keren Zhu
# @date 06/27/2019
# @brief Main file to run the Magical hierarchical flow
#

import os
import sys
import Params, MagicalDB, Flow

class Magical(object):
    def __init__(self, jsonFile):
        self.params = Params.Params()
        self.params.load(jsonFile)
        self.db = MagicalDB.MagicalDB(self.params) # The flow database
        self.db.parse() #parsing the input files
        self.flow = Flow.Flow(self.db)
    def run(self):
        self.flow.run()

if __name__ == "__main__":
    """
    @brief main function to invoke the entire Magical flow
    """

    params = Params.Params()
    params.printWelcome()
    if len(sys.argv) == 1 or '-h' in sys.argv[1:] or '--help' in sys.argv[1:]:
        params.printHelp()
        exit()
    elif len(sys.argv) != 2:
        print("[E] One input parameters in json format in required")
        params.printHelp()
        exit()

    # load parameters 
    params.load(sys.argv[1])
    print("[I] parameters = %s" % (params))

    db = MagicalDB.MagicalDB(params) # The flow database

    db.parse() #parsing the input files

    flow = Flow.Flow(db)
    flow.run()

#Workaround for Pyinstaller. ref:https://github.com/pyinstaller/pyinstaller/issues/2820
if 0: 
    import UserList
    import UserString
    import UserDict
    import itertools
    import collections
    import future.backports.misc
    import commands
    import base64
    import __buildin__
    import math
    import reprlib
    import functools
    import re
    import subprocess
