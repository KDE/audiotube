#!/usr/bin/env bash

set -e

export GIT_CLONE_ARGS="--depth 1 --single-branch"
export FLATPAK_DIR="$(readlink -f $(dirname $0))"
cd ${FLATPAK_DIR}

if [ ! -d flatpak-builder-tools ]; then
        git clone ${GIT_CLONE_ARGS} https://github.com/flatpak/flatpak-builder-tools
else
	git -C flatpak-builder-tools pull
fi

./flatpak-builder-tools/pip/flatpak-pip-generator youtube_dl
./flatpak-builder-tools/pip/flatpak-pip-generator ytmusicapi
