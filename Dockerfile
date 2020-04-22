FROM ubuntu:eoan

ARG LIBMYSQL_DEB=https://repo.mysql.com/apt/debian/pool/mysql-5.7/m/mysql-community/libmysqlclient20_5.7.29-1debian10_amd64.deb
ARG POL_RELEASE=https://github.com/polserver/polserver/releases/download/NightlyRelease/Nightly-Linux-clang.zip
ARG POL_TEMP_DIR=/tmp/poltmp/
ARG POL_DEST=/polserver

RUN apt-get update \
 && apt-get install -y -q --no-install-recommends \
    ca-certificates wget unzip rsync procps \
    libatomic1

RUN mkdir -p $POL_TEMP_DIR \
 && cd  $POL_TEMP_DIR \
 && wget $POL_RELEASE -O pol.zip \
 && unzip pol.zip \
 && rm *dbg.zip \
 && unzip polserver*.zip \
 && rm -rf *.zip \
 && mkdir -p $POL_DEST \
 && mv ./**/* $POL_DEST/ \
 && rm -rf $POL_TEMP_DIR/ \
 && wget $LIBMYSQL_DEB -O /tmp/libmysqlclient20.deb \
 && dpkg --extract /tmp/libmysqlclient20.deb /tmp/libmysqlclient20 \
 && cp /tmp/libmysqlclient20/usr/lib/x86_64-linux-gnu/libmysqlclient.so.20 /usr/lib/x86_64-linux-gnu/libmysqlclient.so.20 \
 && rm -rf /tmp/*

COPY . /app/
WORKDIR /app/

RUN rsync -av $POL_DEST/ . \
 && cd setup \
 && cp -f ./config/servers.cfg ../config/servers.cfg \
 && cp -f ./pol.cfg ../pol.cfg \
 && cp -f $POL_DEST/scripts/ecompile ./ecompile \
 && ./ecompile -C ecompile.cfg


CMD ["./run.sh"]