g++ $1 -o $1.exe -Wextra -Wall -Werror -Wpedantic -std=c++14 -O2 -g
objcopy --only-keep-debug $1.exe $1.debug
strip -g $1.exe
objcopy --add-gnu-debuglink=$1.debug $1.exe

