#!/bin/bash
conan install conanfile.txt -s compiler.runtime=MT -if Build --build=missing
cmake -BBuild . -DCMAKE_BUILD_TYPE=Release
