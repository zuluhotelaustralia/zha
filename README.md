![Compile Status](https://github.com/ZuluHotelAustralia/zha/workflows/Compile%20test/badge.svg)

# README #
### What is this repository for? ###

* Zulu Hotel Australia
* Current version running on POL100

### How do I get set up? ###
* Read the instructions in /setup/README.md

### Development environment ###
You can use the /dev folder to place all your development environment config files, programs and other scripts that wont be checked in.
A recommended development tool (IDE) to use for server scripting is [Visual Studio Code](https://www.visualstudio.com/products/code-vs.aspx) which is a light weight editor with git support.

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Jonathan Morland-Barrett (Dev GIB) - jmorland.barrett@gmail.com
* Craig Bruce (Harlz) - craigbruce32@gmail.com
* (Seravy) - seravy@gmail.com

### Getting started ###
* Windows subsystem for Linux
`sudo apt-get update`
`sudo apt-get install default-libmysqlclient-dev` on ubuntu 18.04LTS (libmysqlclient-dev on ubuntu debian <=8)

### Docker images ###

You can choose to run a bundled version of the server by utilizing the provided [`docker-compose.yml`](./docker-compose.yml). The compose file uses a single volume to store the shard data and symlinks it into the working directory.
The `shard` directory by default is located in the parent directory of the compose file, this is so you can run it from the cloned source repo. If you need to you can edit the path to suit your setup.

The directory structure should look like this
```
shard
├── client    # Copy of UO client for realm-gen, can be removed after the realm is generated
├── config    # Custom configs that will be copied into the servers config folder on startup
├── data      # World data
└── realm     # Realm data generated from the client folder
```

Once the `shard` directory is ready, to start the server you can simply run:
```bash
docker-compose up -d
```

Update to the latest image restart:
```bash
docker-compose pull && docker-compose up -d
```

Stop the server (save first):
```bash
docker-compose down
```

#### POL base image ###
As there's no official docker image for POL we use our own one located in the [polcore](https://github.com/ZuluHotelAustralia/polcore) repository. The image is published as `docker.pkg.github.com/zuluhotelaustralia/polcore/polcore:latest`

### Live Updates with Docker ###
To do live updates to the running scripts on the shard you can copy the `live-update.sh` script to the server to copy in compiled scripts (`*.ecl`) from a newer docker image.

You will need `socat` installed in order to run the script.

**Installation:**
```bash
sudo apt-get install socat
wget https://raw.githubusercontent.com/ZuluHotelAustralia/zha/master/live-update.sh
chmod +x live-update.sh
```

**Run update**
```bash
./live-update <target-branch>
```