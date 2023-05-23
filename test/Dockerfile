FROM ubuntu:latest

SHELL ["/bin/bash", "-c"]

RUN set -euxo pipefail; \
    apt-get -y update; \
    apt-get -y dist-upgrade; \
    apt-get -y install \
      bzip2 \
      curl \
      g++ \
      git-core \
      gpg \
      libssl-dev \
      make \
      python3 \
      python3-dev \
      python3-pip \
      xz-utils; \
    apt-get clean && rm -rf /var/lib/apt/lists/*; \
    python3 -m pip install -U pip; \
    python3 -m pip install -U \
      mahjong==1.1.11 \
      setuptools \
      wheel; \
    useradd -ms /bin/bash ubuntu; \
    mkdir /workspace; \
    chown ubuntu:ubuntu /workspace

USER ubuntu

RUN set -euxo pipefail; \
    pushd /workspace; \
    git clone 'https://github.com/Cryolite/prerequisites'; \
    popd; \
    /workspace/prerequisites/gcc/install --debug --prefix /home/ubuntu/.local
ENV C_INCLUDE_PATH="/home/ubuntu/.local/include"
ENV CPLUS_INCLUDE_PATH="/home/ubuntu/.local/include"
ENV LIBRARY_PATH="/home/ubuntu/.local/lib64:/home/ubuntu/.local/lib"
ENV LD_LIBRARY_PATH="/home/ubuntu/.local/lib64:/home/ubuntu/.local/lib"
ENV PATH="/home/ubuntu/.local/bin${PATH:+:$PATH}"

RUN set -euxo pipefail; \
    /workspace/prerequisites/cmake/install --debug --prefix /home/ubuntu/.local; \
    echo 'import toolset : using ; using python : : /usr/bin/python3 ;' > /home/ubuntu/user-config.jam; \
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

RUN set -euxo pipefail; \
    gunzip -k map.bin.gz; \
    mkdir build; \
    pushd build; \
    cmake -DCMAKE_BUILD_TYPE=Release ..; \
    make -j$(nproc) verbose=1; \
    popd

ENTRYPOINT ["bash", "-c", "time build/test_calculator"]
