from glob import glob
from distutils.core import setup
from Cython.Build import cythonize

setup(
        name="Magical",
        ext_modules=cythonize("python/*.pyx")

)
