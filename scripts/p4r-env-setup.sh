#!/bin/bash
startup_path=$_
if [[ $startup_path != *"/bash" ]]; then
    echo "Please run with bash"
    return
fi
script_folder=$0
script_folder="$( cd "$( dirname "$script_folder" )" >/dev/null 2>&1 && pwd )"
if ! echo $script_folder | grep "scripts" > /dev/null; then
    echo "Please run: bash $0"
    exit 1
fi

if ! grep "/usr/bin/modulecmd" ~/.profile > /dev/null || test $1; then
    profileAddition="
if [ -f /usr/bin/modulecmd ] ; then
    module load gcc/9.1.0
    module load git/2.14.1
    module load vscode/1.32.3
    module load cmake/3.12.0
fi

if [ -f \$HOME/.bashrc ]; then
    source \$HOME/.bashrc
fi
"

    rcAddition="
export PS1='\[\e]0;\u: \w\a\]\[\033[01;32m\]\u\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
export PATH=$script_folder:\$PATH
"

    echo "$profileAddition" >> ~/.profile
    echo "$rcAddition" >> ~/.bashrc
    if [ -f ~/.zshrc ]; then
        echo "$rcAddition" >> ~/.zshrc
    fi

    echo "Success!"
    echo "Now run 'source ~/.profile' and you will be done"
else
    echo "no changes made"
fi
