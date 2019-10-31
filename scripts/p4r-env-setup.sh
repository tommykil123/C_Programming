#!/bin/bash
startup_path=$_
if [[ $startup_path != *"/bash" ]]; then
    echo "Please run with bash"
    return
fi
script_folder=$0
if ! echo $script_folder | grep "scripts" > /dev/null; then
    echo "Please run: bash $0"
    exit 1
fi

if ! grep "/usr/bin/modulecmd" ~/.profile > /dev/null || test $1; then
    profileAddition="
if [ -f /usr/bin/modulecmd ] ; then
    module load gcc/7.1.0
    module load git/2.14.1
    module load vscode/1.32.3
fi"
    echo "$profileAddition" >> ~/.profile
    dir="$( cd "$( dirname "$script_folder" )" >/dev/null 2>&1 && pwd )"
    rcAdd="export PATH=$dir:\$PATH"
    echo "$rcAdd" >> ~/.bashrc
    if [ -f ~/.zshrc ]; then
        echo "$rcAdd" >> ~/.zshrc
    fi
    eval "$profileAddition"
    eval "$rcAdd"
else
    dir="$( cd "$( dirname "$script_folder" )" >/dev/null 2>&1 && pwd )"
    echo "no changes made"
fi
