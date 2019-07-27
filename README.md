# Tubemiter
Fits circular tubes

ersion 1.3 (Dec 2008) - added DXF saving.
Version 1.4 (Apr 2009) - added inch/metric units.
Version 1.5 (Jun 2010) - tweaked display to handle large tube diameters better.

tubemiter.zip contains a prebuilt executable for Windows.

A tube mitering program for Windows to print on any printer.
The other one is a Hypercard application for Macintosh,
and once I dug up a Mac and a PostScript printer, it was very
useful. However, it doesn't take wall thickness into account,
which is important when doing small angles or equal sized
tubes. It can also deal with tubes meeting larger ones such
that the centrelines are offset.

My program also deals in metric or imperial units and will
translate between them.

File/Settings... puts up a dialog box. You input:

* Diameter of tube being mitered
* The wall thickness of this tube
* The second tube diameter (the one it abuts up to)
* The included angle in degrees
* The offset between the tube axes
* Whether the dimensions are in mm or inches.

It paints the picture on the window, and File/Print... prints it
on any connected printer. File/Save to DXF... allows you to save 
the drawing to a DXF file.

There are two curves - the dotted one is the template you
would cut if the wall thickness were zero (similar to the
HyperCard program's output) and the solid curve is the template
corresponding to the requested wall thickness. In a DXF file,
the solid curve is in Layer 0 and the dotted curve is in Layer 1.

If the mitered tube diameter is greater than the second diameter,
then the second tube passes through two holes in the mitered tube.
This situation is handled by continuing the template on the
other side. Small gaps may appear in the curves; this is a known
"feature" of the way the program works.


