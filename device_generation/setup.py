import setuptools

setuptools.setup(
    author="Mingjie Liu",
    author_email="jay_liu@utexas.edu",
    name='device_generation',
    description='Device generation for MAGICAL',
    version='0.0.1',
    packages=setuptools.find_packages(),
    classifiers=[
        'Development Status :: 4 - Beta',
        'Programming Language :: Python',
    ],
    py_modules=["basic","glovar","Pin","Mosfet","Resistor","Capacitor"],
)
