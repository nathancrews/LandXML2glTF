# LandXML2glTF
Converts LandXML file format, versions 2.0 and 1.2, to .glTF files for 3D viewing integration

Prerequisites:

The GDAL (PROJ) library is used for WGS84 geolocation and requires evironment variable PROJ_LIB pointing to the location for the requird proj.db file. A proj.db file is provided in the \LXML2glTF\data directory.

Example: PROJ_LIB="D:\GitHub\LandXML2glTF\LXML2glTF\data"

Building with Visual Studio for x64:

Open the LandXML2glTF\LXML2glTF\LandXML2glTF.sln solution file and build release or debug x64 targets.
(CMake config for MacOS and Linux to follow)

Usage:
LXML2glTF.exe [path to LandXML file] [/s]

Required: [path to LandXML file] full or relative path to LandXML file. If it contains a trailing wild char ".\LandXML\*", all .xml files in that directory will be converted.

Optional: /s If specified sub-directories will be searched for .xml for conversion.

Example Usage 1:

LXML2glTF.exe "..\..\..\..\LandXML\KYRoad.xml"

Output:
Converting "..\\..\\..\\..\\LandXML\\KYROAD\\kyroad.xml" to "D:\\GitHub\\LandXML2glTF\\LandXML\\KYROAD\\kyroad.gltf"
Parsing and building LandXML model...
Building glTF model...
Writing glTF file: D:\GitHub\LandXML2glTF\LandXML\KYROAD\kyroad.gltf

Example Usage 2:

D:\github\landxml2gltf>.\LXML2glTF\bin\x64\Release\LXML2glTF.exe ".\LandXML\*" /s

Output:

{This tests an invalid LandXML file} Converting [1 of 5] "D:\\github\\landxml2gltf\\LandXML\\DefaultTexture.xml" to "D:\\github\\landxml2gltf\\LandXML\\DefaultTexture.gltf"
error: The LandXML file is missing the required <Units> element.Error: failed to parse LandXML data from file: D:\github\landxml2gltf\LandXML\DefaultTexture.xml
Converting [2 of 5] "D:\\github\\landxml2gltf\\LandXML\\KYRoad\\KYRoad.xml" to "D:\\github\\landxml2gltf\\LandXML\\KYRoad\\KYRoad.gltf"
Parsing and building LandXML model...
Building glTF model...
Writing glTF file: D:\github\landxml2gltf\LandXML\KYRoad\KYRoad.gltf
Converting [4 of 5] "D:\\github\\landxml2gltf\\LandXML\\subdivision\\subdivision.xml" to "D:\\github\\landxml2gltf\\LandXML\\subdivision\\subdivision.gltf"
Parsing and building LandXML model...
error: Unable to find LandXML <MaterialTable> element in ./data/DefaultTexture.xmlError: failed to load and parse valid LandXML data from file: D:\github\landxml2gltf\LandXML\subdivision\subdivision.xml
Converting [5 of 5] "D:\\github\\landxml2gltf\\LandXML\\subdivision-2.0\\subdivision-2.0.xml" to "D:\\github\\landxml2gltf\\LandXML\\subdivision-2.0\\subdivision-2.0.gltf"
Parsing and building LandXML model...
Building glTF model...
Writing glTF file: D:\github\landxml2gltf\LandXML\subdivision-2.0\subdivision-2.0.gltf
