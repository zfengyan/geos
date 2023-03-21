This is the repository forked from:

https://github.com/vmx/geos

I happened to use a feature of GEOS library called "polygonize" - which is, take a set of geometries (such as line strings) as input (can be unordered), and output the polygons - at the same
time omitting the possibly dangling parts (such as dangling lines, points, etc.).

The feature is amazing yet the tutorial is hard to find.

I was inspired from the example code given by the original repository, yet I found it a bit old and especially for C++ API. So I will add my testing code, as a heads-up, also 
hope to help someone, somewhere else.

The testing code snipped will be in [examples/My_example](https://github.com/zfengyan/geos/tree/master/examples/My_example).

=============================

The original information is as follows:

### BUILDING, TESTING, INSTALLING
=============================

Unix
----

Using Autotools:
  ./autogen.sh  # in ${srcdir}, if obtained from SVN
  ${srcdir}/configure # in build dir

Using CMake:
  cmake ${srcdir} # in build dir

Now, all versions:
  make
  make check
  make install # as root
  ldconfig # as root

Win32
-----

If you use Microsoft Visual C++ (7.1 or later) compiler, you can build 
GEOS using NMAKE program and provided makefile.vc files.

If you are building from SVN checkout, first run: autogen.bat
Then:

  nmake /f makefile.vc MSVC_VER=1400

where 1400 is version number of Visual C++ compiler, here Visual C++ 8.0 
from Visual Studio 2005 (supported versions are 1300, 1310, 1400 and 1500). 
The bootstrap.bat step is required to generate a couple of header files.

In order to build debug configuration of GEOS, additional flag DEBUG=1 
is required:

  nmake /f makefile.vc MSVC_VER=1400 DEBUG=1


CLIENT APPLICATIONS
===================

Using the C interface (recommended)
-----------------------------------

To compile programs against the C lib (recommended):

  CFLAGS += `geos-config --cflags`
  LDFLAGS += `geos-config --ldflags` -lgeos_c
  #include <geos_c.h>

Example usage:

  capi/geostest.c contains basic usage examples.

Using the C++ interface (discouraged)
-------------------------------------

To compile programs against the C++ lib:

  CFLAGS += `geos-config --cflags`
  LDFLAGS += `geos-config --ldflags` -lgeos
  #include <geos.h>

Example usage:

  doc/example.cpp contains basic usage examples.

Scripting language bindings
---------------------------

Ruby bindings are fully supported. To build, use the --enable-ruby option
when configuring:

  ./configure ... --enable-ruby

PHP bindings are fully supported. To build, use the --enable-php option
when configuring:

  ./configure ... --enable-php
    
Since version 3.0, the Python bindings are unsupported. Recommended options:

 1. Become or recruit a new maintainer.
 2. Use Shapely (http://pypi.python.org/pypi/Shapely) with Python
    versions 2.4 or greater.
 3. Simply call functions from libgeos_c via Python ctypes.

DOCUMENTATION
=============

  cd doc; make doxygen-html

  http://trac.osgeo.org/geos
