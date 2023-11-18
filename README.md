# LandXML2glTF
Converts LandXML file format, versions 2.0 and 1.2, to .glTF files for 3D viewing integration

Prerequisites:

The GDAL (PROJ) library is used for WGS84 geolocation and requires evironment variable PROJ_LIB pointing to the location for the requird proj.db file. A proj.db file is provided in the \LXML2glTF\data directory.

Example: PROJ_LIB="D:\GitHub\LandXML2glTF\LXML2glTF\data"

Building with Visual Studio for x64:

Open the LandXML2glTF\LXML2glTF\LandXML2glTF.sln solution file and build release or debug x64 targets.

