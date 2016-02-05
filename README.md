Wavelet stego
=============

Description
-----------

Simple GUI application with ability to hide text into images using wavelet transform.

Main feature doesn't work stable yet! Sometimes hidden text can not be readed correctly.

Only png files with ARGB32 format are supported.


Dependencies
------------

* GNU Scientific Library (`libgsl-dev`)
* gtkmm 3 (`libgtkmm-3.0-dev`)
* cmake

Build instruction
-----------------

First install Dependencies

	$ git clone https://github.com/rph-pl/wavelet-stego.git
	$ cd wavelet-stego
	$ mkdir build
	$ cd build
	$ cmake ..
	$ make

After building you can find executable file **wavelet\_stego** in build directory.





