FROM docker.pkg.github.com/zuluhotelaustralia/zha/pol:latest AS polserver

FROM ubuntu:eoan

WORKDIR /app/
COPY . .
COPY --from=polserver /app .

RUN apt-get update \
 && apt-get install -y -q --no-install-recommends \
    ca-certificates wget unzip rsync procps \
    libatomic1 \
 && apt-get clean \
 && rm -r /var/lib/apt/lists/*

RUN cp -f setup/config/servers.cfg ./config/servers.cfg \
 && cp -f setup/pol.cfg ./pol.cfg \
 && cd setup \
 && ../scripts/ecompile -C ecompile.cfg

CMD ["./run.sh"]