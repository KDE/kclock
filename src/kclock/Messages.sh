#! /bin/sh

# Copyright 2020 Yuri Chornoivan
# SPDX-License-Identifier: GPL-2.0-or-later

$XGETTEXT `find . -name \*.cpp -o -name \*.h -o -name \*.qml` -o $podir/kclock.pot
