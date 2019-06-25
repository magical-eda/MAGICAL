"""
@brief run all unittest
@author Keren Zhu
@date 06/25/2019
"""

import unittest

loader = unittest.TestLoader()
cpp_api_test = "./cpp_api_test"
suite = loader.discover(cpp_api_test)

runner = unittest.TextTestRunner()
runner.run(suite)
