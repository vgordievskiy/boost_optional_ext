#!/bin/bash
conan install conanfile.txt -if Build --build=missing
cmake -G Xcode -BBuild .
