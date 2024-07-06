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
    bzip2 \
    cmake \
    curl \
    g++ \
    git-core \
    git-lfs \
    gpg \
    libbz2-dev \
    libffi-dev \
    liblzma-dev \
    libncurses-dev \
    libreadline-dev \
    libsqlite3-dev \
    libssl-dev \
    make \
    xz-utils \
    zlib1g-dev
sudo apt-get clean
sudo rm -rf /var/lib/apt/lists/*
sudo chown -R vscode:vscode /workspaces

pushd /workspaces
git clone https://github.com/Cryolite/prerequisites
popd

/workspaces/prerequisites/gcc/install --debug --prefix "$HOME/.local"
export CC="$HOME/.local/bin/gcc"
export CXX="$HOME/.local/bin/g++"

/workspaces/prerequisites/cmake/install --debug --prefix "$HOME/.local"

curl https://pyenv.run | bash
export PYENV_ROOT="$HOME/.pyenv"
export PATH="$PYENV_ROOT/bin${PATH:+:$PATH}"
PYTHON_LATEST_VERSION=$(pyenv install -l | grep -Eo '^[[:space:]]+[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+$' | grep -Eo '[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+' | LANG=C.UTF-8 sort -V | tail -n 1)
PYTHON_LATEST_VERSION_MAJOR=$(echo $PYTHON_LATEST_VERSION | grep -Eo '^[[:digit:]]+\.[[:digit:]]+')
pyenv install $PYTHON_LATEST_VERSION
pyenv global $PYTHON_LATEST_VERSION
pushd "$PYENV_ROOT/versions/$PYTHON_LATEST_VERSION/lib"
ln -s libpython$PYTHON_LATEST_VERSION_MAJOR.so libpython.so
popd
eval "$(pyenv init -)"
PYTHON_PREFIX="$(python3 -c 'import sys; print(sys.prefix);')"
python3 -m pip install -U pip
python3 -m pip install -U \
  mahjong==1.1.11 \
  setuptools \
  wheel

echo "import toolset : using ; using python : : \"$PYTHON_PREFIX/bin/python3\" ;" > "$HOME/user-config.jam"
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
