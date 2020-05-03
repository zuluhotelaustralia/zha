#!/bin/bash -e

owner="zuluhotelaustralia"
repo="zha"
image="server"
branch="${1:-latest}"
container="polserver"
remote="docker.pkg.github.com/${owner}/${repo}/${image}:${branch}"

echo "Live updating scripts via docker"
# Pull the remote branch docker image to extract the files from
docker pull $remote

# tar the ecl's and pipe them across to the currently running container
docker run --rm -i $remote bash -c 'tar -czf - $(find . -type f -name "*\.ecl")' | docker exec -i $container bash -c 'tar zxf -'

# Unload the scripts
# send unlock, unload (Su) sequence twice in-case the console was already unlocked
echo "SuSu" | socat EXEC:"docker attach $container",pty STDIN
echo "Scripts updated and reloaded. Don't forget to merge your fixes upstream!"