#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

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
