rem converts all files in LandXML example directory and sub-directories to .gltf
cd .\LXML2glTF\bin\x64\Release\
.\LXML2glTF.exe "../../../../LandXML/*" /s

rem for testing 2k+ sample LandXML files
rem .\LXML2glTF.exe "E:\LandXML\Uploaded Files\*" /s
rem .\LXML2glTF.exe "E:\LandXML\Sample Files\*" /s
rem .\LXML2glTF.exe "E:\LandXML\Schema Sample Files\*" /s
rem .\LXML2glTF.exe "E:\adeskProjectData\*" /s
