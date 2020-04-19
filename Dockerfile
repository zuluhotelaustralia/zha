FROM debian:stable-slim

ARG LIBMYSQL_DPKG=https://repo.mysql.com/apt/debian/pool/mysql-5.7/m/mysql-community/libmysqlclient20_5.7.29-1debian10_amd64.deb
ARG POL_RELEASE=https://github.com/polserver/polserver/releases/download/NightlyRelease/Nightly-Linux-gcc.zip

RUN apt-get update \
 && apt-get install -y -q --no-install-recommends \
    ca-certificates wget unzip rsync procps \
    libatomic1

RUN cd /tmp && \
 wget $POL_RELEASE -O pol.zip \
 && unzip pol.zip \
 && rm *dbg.zip \
 && unzip polserver*.zip \
 && rm -rf *.zip \
 && mkdir -p /polserver/ \
 && mv ./**/* /polserver/ \
 && wget $LIBMYSQL_DPKG -O /tmp/libmysqlclient20.dpkg \
 && dpkg --extract /tmp/libmysqlclient20.dpkg /tmp/libmysqlclient20 \
 && cp /tmp/libmysqlclient20/usr/lib/x86_64-linux-gnu/libmysqlclient.so.20 /usr/lib/x86_64-linux-gnu/libmysqlclient.so.20 \
 && rm -rf /tmp/*

COPY . /app/
WORKDIR /app/


RUN rsync -av /polserver/ . \
 && cp -f setup/config/servers.cfg config/servers.cfg \
 && cp -f setup/pol.cfg pol.cfg \
 && mv -f setup/data data \
 && rm -rf /polserver \
 && cp setup/ecompile.cfg scripts/ecompile.cfg \
 && scripts/ecompile -C ./scripts/ecompile.cfg


CMD ["./run.sh"]