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
    git \
    git-lfs \
    libbz2-dev \
    libffi-dev \
    liblzma-dev \
    libncurses-dev \
    libreadline-dev \
    libsqlite3-dev \
    libssl-dev \
    zlib1g-dev
sudo apt-get clean
sudo rm -rf /var/lib/apt/lists/*
sudo chown -R vscode:vscode /workspaces

pushd /workspaces
git clone https://github.com/Cryolite/prerequisites
popd

/workspaces/prerequisites/gcc/install --debug --prefix "$HOME/.local"
echo 'export C_INCLUDE_PATH="$HOME/.local/include${C_INCLUDE_PATH:+:$C_INCLUDE_PATH}"' >> "$HOME/.bashrc"
echo 'export C_INCLUDE_PATH="$HOME/.local/include${C_INCLUDE_PATH:+:$C_INCLUDE_PATH}"' >> "$HOME/.profile"
export C_INCLUDE_PATH="$HOME/.local/include${C_INCLUDE_PATH:+:$C_INCLUDE_PATH}"
echo 'export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"' >> "$HOME/.bashrc"
echo 'export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"' >> "$HOME/.profile"
export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
echo 'export LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"' >> "$HOME/.bashrc"
echo 'export LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"' >> "$HOME/.profile"
export LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"
echo 'export LD_LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"' >> "$HOME/.bashrc"
echo 'export LD_LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"' >> "$HOME/.profile"
export LD_LIBRARY_PATH="$HOME/.local/lib64:$HOME/.local/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
echo 'export CC="$HOME/.local/bin/gcc"' >> "$HOME/.bashrc"
echo 'export CC="$HOME/.local/bin/gcc"' >> "$HOME/.profile"
export CC="$HOME/.local/bin/gcc"
echo 'export CXX="$HOME/.local/bin/g++"' >> "$HOME/.bashrc"
echo 'export CXX="$HOME/.local/bin/g++"' >> "$HOME/.profile"
export CXX="$HOME/.local/bin/g++"
echo 'export PATH="$HOME/.local/bin${PATH:+:$PATH}"' >> "$HOME/.bashrc"
echo 'export PATH="$HOME/.local/bin${PATH:+:$PATH}"' >> "$HOME/.profile"
export PATH="$HOME/.local/bin${PATH:+:$PATH}"

/workspaces/prerequisites/cmake/install --debug --prefix "$HOME/.local"

curl https://pyenv.run | bash
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> "$HOME/.bashrc"
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> "$HOME/.profile"
export PYENV_ROOT="$HOME/.pyenv"
echo 'export PATH="$PYENV_ROOT/bin${PATH:+:$PATH}"' >> "$HOME/.bashrc"
echo 'export PATH="$PYENV_ROOT/bin${PATH:+:$PATH}"' >> "$HOME/.profile"
export PATH="$PYENV_ROOT/bin${PATH:+:$PATH}"
PYTHON_LATEST_VERSION=$(pyenv install -l | grep -Eo '^[[:space:]]+[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+$' | grep -Eo '[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+' | LANG=C.UTF-8 sort -V | tail -n 1)
PYTHON_LATEST_VERSION_MAJOR=$(echo $PYTHON_LATEST_VERSION | grep -Eo '^[[:digit:]]+\.[[:digit:]]+')
pyenv install $PYTHON_LATEST_VERSION
pyenv global $PYTHON_LATEST_VERSION
pushd "$PYENV_ROOT/versions/$PYTHON_LATEST_VERSION/lib"
ln -s libpython$PYTHON_LATEST_VERSION_MAJOR.so libpython.so
popd
echo 'eval "$(pyenv init -)"' >> "$HOME/.bashrc"
echo 'eval "$(pyenv init -)"' >> "$HOME/.profile"
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
