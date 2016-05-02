#!/bin/sh

ls *.cc *.cpp | xargs clang-format -i
