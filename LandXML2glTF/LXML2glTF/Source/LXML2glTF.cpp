// LXML2glTF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifdef _WIN32
#include "Windows.h"
#endif

#include <cstdlib>
#include <iostream>
#include <cmath>
#include "LandXML2glTFDll.h"

int main()
{
    std::cout << "Hello World!\n";

    LandXMLModel *newFoo = new LandXMLModel;

    newFoo->run();
}


