##
# @file Flow.py
# @author Keren Zhu
# @date 06/27/2019
# @brief Main file to run the Magical hierarchical flow
#

import os
import sys
import Params

if __name__ = "__main__":
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

