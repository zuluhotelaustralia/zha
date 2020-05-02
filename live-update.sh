#!/bin/bash -e

owner="zuluhotelaustralia"
repo="zha"
image="server"
branch="${1:-latest}"
container="polserver"

remote="docker.pkg.github.com/${owner}/${repo}/${image}:${branch}"


docker pull $remote
docker run --rm -i $remote cat ecl-archive.tgz | docker exec -i $container tar zxvf -
