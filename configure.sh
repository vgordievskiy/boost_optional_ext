#!/bin/bash
conan install conanfile.txt -if Build --build=missing
cmake -BBuild .
ln -s ./Build/compile_commands.json compile_commands.json
