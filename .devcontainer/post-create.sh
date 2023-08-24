#!/usr/bin/env bash

set -euxo pipefail

PS4='+${BASH_SOURCE[0]}:$LINENO: '
if [[ -t 1 ]] && type -t tput >/dev/null; then
  if (( "$(tput colors)" == 256 )); then
    PS4='$(tput setaf 10)'$PS4'$(tput sgr0)'
  else
    PS4='$(tput setaf 2)'$PS4'$(tput sgr0)'
  fi
fi

# Install prerequisite packages.
sudo apt-get -y update
sudo apt-get -y dist-upgrade
sudo apt-get -y install \
  g++ \
  git-lfs \
  python3 \
  python3-dev

git lfs install

sudo chown vscode:vscode /workspaces

# Install prerequisite Python packages.
python3 -m pip install -U pip
python3 -m pip install -U \
  mahjong==1.1.11 \
  setuptools \
  wheel

# Clone `prerequisites`.
pushd /workspaces
git clone 'https://github.com/Cryolite/prerequisites'
popd

# Install GCC.
/workspaces/prerequisites/gcc/install --debug --prefix "$HOME/.local"
if [[ -v C_INCLUDE_PATH ]]; then
  OLD_C_INCLUDE_PATH="$C_INCLUDE_PATH"
fi
export C_INCLUDE_PATH="$HOME/.local/include${C_INCLUDE_PATH:+:$C_INCLUDE_PATH}"
if [[ -v CPLUS_INCLUDE_PATH ]]; then
  OLD_CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH"
fi
export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
if [[ -v LIBRARY_PATH ]]; then
  OLD_LIBRARY_PATH="$LIBRARY_PATH"
fi
export LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"
if [[ -v LD_LIBRARY_PATH ]]; then
  OLD_LD_LIBRARY_PATH="$LD_LIBRARY_PATH"
fi
export LD_LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
if [[ -v PATH ]]; then
  OLD_PATH="$PATH"
fi
export PATH="$HOME/.local/bin${PATH:+:$PATH}"

# Install CMake.
/workspaces/prerequisites/cmake/install --debug --prefix "$HOME/.local"

# Install Boost.Stacktrace and Boost.Python.
echo 'import toolset : using ; using python : : /usr/local/python/current/bin/python3 ;' > "$HOME/user-config.jam"
/workspaces/prerequisites/boost/download --debug --source-dir /workspaces/boost
/workspaces/prerequisites/boost/build --debug --source-dir /workspaces/boost --prefix "$HOME/.local" -- \
  -d+2 --with-headers --with-timer --with-python --build-type=complete --layout=tagged \
  toolset=gcc variant=debug threading=multi link=shared runtime-link=shared \
  cxxflags=-D_GLIBCXX_DEBUG cxxflags=-D_GLIBCXX_DEBUG_PEDANTIC \
  cflags=-fsanitize=address cxxflags=-fsanitize=address linkflags=-fsanitize=address \
  cflags=-fsanitize=undefined cxxflags=-fsanitize=undefined linkflags=-fsanitize=undefined
/workspaces/prerequisites/boost/build --debug --source-dir /workspaces/boost --prefix "$HOME/.local" -- \
  -d+2 --with-headers --with-timer --with-python --build-type=complete --layout=tagged \
  toolset=gcc variant=release threading=multi link=shared runtime-link=shared
rm -rf /workspaces/boost
