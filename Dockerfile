FROM ubuntu:latest

SHELL ["/bin/bash", "-c"]

RUN set -euxo pipefail; \
    apt-get -y update; \
    apt-get -y dist-upgrade; \
    apt-get -y install \
      bzip2 \
      cmake \
      curl \
      g++ \
      git-core \
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
      zlib1g-dev; \
    apt-get clean && rm -rf /var/lib/apt/lists/*; \
    mkdir /workspace; \
    chown ubuntu:ubuntu /workspace

USER ubuntu

RUN set -euxo pipefail; \
    curl https://pyenv.run | bash
ENV PYENV_ROOT="/home/ubuntu/.pyenv"
ENV PATH="$PYENV_ROOT/bin${PATH:+:$PATH}"

RUN set -euxo pipefail; \
    PYTHON_LATEST_VERSION=$(pyenv install -l | grep -Eo '^[[:space:]]+[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+$' | grep -Eo '[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+' | LANG=C.UTF-8 sort -V | tail -n 1); \
    PYTHON_LATEST_VERSION_MAJOR=$(echo $PYTHON_LATEST_VERSION | grep -Eo '^[[:digit:]]+\.[[:digit:]]+'); \
    pyenv install "$PYTHON_LATEST_VERSION"; \
    pyenv global "$PYTHON_LATEST_VERSION"; \
    pushd /home/ubuntu/.pyenv/versions/$PYTHON_LATEST_VERSION/lib; \
    ln -s libpython$PYTHON_LATEST_VERSION_MAJOR.so libpython.so; \
    popd; \
    eval "$(pyenv init -)"; \
    PYTHON_PREFIX="$(python3 -c 'import sys; print(sys.prefix);')"; \
    python3 -m pip install -U pip; \
    python3 -m pip install -U \
      mahjong==1.1.11 \
      setuptools \
      wheel; \
    pushd /workspace; \
    git clone 'https://github.com/Cryolite/prerequisites'; \
    popd; \
    echo "import toolset : using ; using python : : \"$PYTHON_PREFIX/bin/python3\" ;" > /home/ubuntu/user-config.jam; \
    /workspace/prerequisites/boost/download --debug --source-dir /workspace/boost; \
    /workspace/prerequisites/boost/build --debug --source-dir /workspace/boost --prefix /home/ubuntu/.local -- \
      -d+2 --with-headers --with-timer --with-python --build-type=complete --layout=tagged \
      toolset=gcc variant=debug threading=multi link=shared runtime-link=shared \
      cxxflags=-D_GLIBCXX_DEBUG cxxflags=-D_GLIBCXX_DEBUG_PEDANTIC \
      cflags=-fsanitize=address cxxflags=-fsanitize=address linkflags=-fsanitize=address \
      cflags=-fsanitize=undefined cxxflags=-fsanitize=undefined linkflags=-fsanitize=undefined; \
    /workspace/prerequisites/boost/build --debug --source-dir /workspace/boost --prefix /home/ubuntu/.local -- \
      -d+2 --with-headers --with-timer --with-python --build-type=complete --layout=tagged \
      toolset=gcc variant=release threading=multi link=shared runtime-link=shared; \
    rm -rf /workspace/boost

COPY --chown=ubuntu:ubuntu . /workspace/tsumonya

WORKDIR /workspace/tsumonya
