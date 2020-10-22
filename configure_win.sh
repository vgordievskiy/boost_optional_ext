#!/bin/bash
conan install conanfile.txt -s compiler.runtime=MD -if Build --build=missing
cmake -BBuild . -DCMAKE_BUILD_TYPE=Release
