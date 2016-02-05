#!/bin/bash

find -name "*.cpp" -o -name "*.hpp" | xargs astyle --options=astylerc -n



