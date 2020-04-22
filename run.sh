#!/bin/bash

set -e

if [ ! -d ../shard ]; then
    echo "Missing shard storage in parent directory"
    exit 1
fi

ln -s -f ../shard/realm .
ln -s -f ../shard/data .

if [ ! -d ./realm/britannia ]; then
    echo "Generating realm files as they do not exist"


    ln -s -f "$(pwd)/../shard/client" setup/uo-client

    pushd setup; ./generate_data_files.sh; popd

    echo "Storing realm cfg in custom config directory"
    cp -f config/multis.cfg ../shard/config/
    cp -f config/tiles.cfg ../shard/config/
    cp -f config/landtiles.cfg ../shard/config/
fi

if [ -n "${SERVER_EXTERNAL_IP}" ]; then
    sed -i "s/--ip--/$SERVER_EXTERNAL_IP/g" ./config/servers.cfg
    echo "Server IP set to $SERVER_EXTERNAL_IP"
fi

if [ -d ../shard/config ]; then
    echo "Copying custom configs"
    cp -f ../shard/config/*.cfg ./config/
fi

./pol
