#!/bin/bash

python2 setup.py build_ext --inplace
rm -r dist
cd python
python2 -m PyInstaller --onefile Magical.py
mv dist ../.
cd ..
