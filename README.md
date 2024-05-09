# LandXML2glTF
Converts LandXML files, versions 2.0 and 1.2, to .glTF files for 3D viewing integration. This project is setup using CMake for Windows, Linux and MacOS.
For assistance on build instructions: nathan@nathancrews.com

<h2>Build Instructions</h2>
Use CMake to generate the build files. On Windows, a Visual Studio solution and project files will be produced.
On Linux, Makefile will be produced. On MacOS, an Xcode project will be produced.

Example: Open Windows Powershell, VSCode terminal or any other terminal that can run cmake, then:

c:\\>d:

d:\\>CD GitHub\\LandXML2glTF

d:\\GitHub\\LandXML2glTF>mkdir build

d:\\GitHub\\LandXML2glTF>CD build

<b>[Windows and Linux]</b>

d:\\GitHub\\LandXML2glTF\build>cmake ..

<b>[MacOS]</b>

d:\\GitHub\\LandXML2glTF\build>cmake -G Xcode ..

Assuming no errors, when cmake completes, the \build\LandXML2GLTF.xxx (or Makefile) project files should exist.

<b>CMake build for Windows, MacOs and Linux:</b>

d:\\GitHub\\LandXML2glTF\build>cmake --build .

<h3>Windows, Open in Visual Studio for x64:</h3>
Open the \build\LandXML2glTF.sln solution file and build release or debug x64 targets.
Once build is complete, the debug and release exe's are located in \landxml2gltf\build\debug\ and \landxml2gltf\build\release\ respectively.

<h3>MacOS, Open in Xcode:</h3>
Open the \build\LANDXML2GLTF.xcodeproj project file and build release or debug x64 targets.

<h2>Usage:</h2>
.LXML2glTF.exe [path to LandXML file] [/s]

<b>Required:</b> [path to LandXML file] full or relative path to the LandXML file(s). If the path contains a trailing wild char ".\\LandXML\\*", all .xml files in that directory will be converted.

<b>Optional:</b> /s If specified sub-directories will be searched for .xml for conversion.
                 /b If specified valid LandXML .xml files will be converted to .glb files.

Example Usage 1:

.LXML2glTF.exe "..\\..\\..\\..\\LandXML\subdivision-2.0.xml"

Output:
Converting "..\\..\\..\\..\\LandXML\\subdivision-2.0\\subdivision-2.0.xml" to "D:\\GitHub\\LandXML2glTF\\LandXML\\subdivision-2.0\\subdivision-2.0.gltf"
Parsing and building LandXML model...
Building glTF model...
Writing glTF file: D:\GitHub\LandXML2glTF\LandXML\subdivision-2.0\subdivision-2.0.gltf

Example Usage 2:

D:\github\landxml2gltf>.\Release\LXML2glTF.exe ".\\LandXML\\*" /s /b

Output:

{This tests an invalid LandXML file} Converting [1 of 3] "D:\\github\\landxml2gltf\\LandXML\\DefaultTexture.xml" to "D:\\github\\landxml2gltf\\LandXML\\DefaultTexture.glb"
Error: The LandXML file is missing the required <Units> element.
Error: failed to parse LandXML data from file: D:\github\landxml2gltf\LandXML\DefaultTexture.xml

Converting [2 of 3] "D:\\github\\landxml2gltf\\LandXML\\olympus\\olympus-new.xml" to "D:\\github\\landxml2gltf\\LandXML\\olympus\\olympus-new.glb"
Parsing and building LandXML model...
Building glTF model...
Writing glTF file: D:\github\landxml2gltf\LandXML\olympus\olympus-new.glb

Converting [3 of 3] "D:\\github\\landxml2gltf\\LandXML\\subdivision-2.0\\subdivision-2.0.xml" to "D:\\github\\landxml2gltf\\LandXML\\subdivision-2.0\\subdivision-2.0.glb"
Parsing and building LandXML model...
Building glTF model...
Writing glTF file: D:\github\landxml2gltf\LandXML\subdivision-2.0\subdivision-2.0.glb


<h2>Future Prerequisites:</h2>

The GDAL (PROJ) library is used for WGS84 geolocation and requires evironment variable PROJ_LIB pointing to the location for the requird proj.db file. A proj.db file is provided in the \LXML2glTF\data directory.

Example: PROJ_LIB="D:\GitHub\LandXML2glTF\LXML2glTF\data"
