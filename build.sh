#!/bin/bash

src=test.c
exe=spxtest
header=spxplot.h
installpath=/usr/local/include

cc=gcc
std=-std=c89
opt=-O2
inc=-I.
lib=-lm

wflag=(
    -Wall
    -Wextra
    -pedantic
)

cmd() {
    echo "$@" && $@
}

compile() {
    cmd $cc $1 -o $2 $std $opt ${wflag[*]} $inc $lib
}

cleanf() {
    [ -f $1 ] && cmd rm -f $1
}

install() {
    [ "$EUID" -ne 0 ] && echo "run with 'sudo' to install" && exit
    cmd cp $header $installpath
    echo "successfully installed $header"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "run with 'sudo' to uninstall" && exit
    cleanf $installpath/$header
    echo "successfully uninstalled $header"
    return 0
}

fail() {
    echo "file '$1' not found"
}

usage() {
    echo "$0 usage:"
    echo -e "\t\t: Compile test.c file with $header"
    echo -e "<source>\t: Compile <source> file with $header"
    echo -e "help\t\t: Print usage information and available commands"
    echo -e "clean\t\t: Delete compiled executables"
    echo -e "install\t\t: Install $header in $installpath (run with sudo)"
    echo -e "uninstall\t: Unnstall $header from $installpath (run with sudo)"
}

(( $# < 1 )) && compile $src $exe && exit

case "$1" in
    "help")
        usage;;
    "clean")
        cleanf output.ppm
        cleanf a.out
        cleanf $exe;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        [ -f $1 ] && compile $1 a.out || fail $1;;
esac
