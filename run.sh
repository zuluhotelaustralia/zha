#!/bin/bash

if [ ! -d ./realm/britannia ]; then
    echo "Generating realm files as they do not exist"
    pushd setup; ./generate_data_files.sh; popd

    echo "Storing realm cfg in custom config directory"
    cp -f config/multis.cfg /shard/config/
    cp -f config/tiles.cfg /shard/config/
    cp -f config/landtiles.cfg /shard/config/
fi

if [ -n "${SERVER_EXTERNAL_IP}" ]; then
    sed -i "s/--ip--/$SERVER_EXTERNAL_IP/g" /app/config/servers.cfg
    echo "Server IP set to $SERVER_EXTERNAL_IP"
fi

if [ -d /shard/config ]; then
    echo "Copying custom configs"
    cp -f /shard/config/* /app/config/
fi

cd /app && ./pol
