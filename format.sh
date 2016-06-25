#!/bin/sh

ls *.h *.hpp *.hh *.cc *.cpp | xargs clang-format -i
