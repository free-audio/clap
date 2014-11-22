#! /bin/bash

wget http://download.qt-project.org/official_releases/qt/5.3/5.3.2/single/qt-everywhere-opensource-src-5.3.2.tar.xz
rm -rf qt
tar xf qt-everywhere-opensource-src-5.3.2.tar.xz
mv qt-everywhere-opensource-src-5.3.2 qt
mkdir qt-build
cd qt-build
../qt/configure -release \
                -opensource \
                -c++11 \
                -static \
                -process \
                -largefile \
                -no-glib \
                -no-pulseaudio \
                -no-alsa \
                -no-cups \
                -no-nis \
                -no-dbus \
                -no-directfb \
                -no-kms \
                -no-linuxfb \
		-no-sql-{mysql,odbc,sqlite,pgsql}
