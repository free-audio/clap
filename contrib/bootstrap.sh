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
		-no-sql-{mysql,odbc,sqlite} \
                -no-compile-examples \
                -no-gtkstyle \
                -skip location \
                -skip serialport \
                -skip xmlpatterns \
                -skip activeqt \
                -skip connectivity \
                -skip androidextras \
                -skip websockets \
                -skip doc \
                -skip multimedia \
                -skip sensors \
                -skip svg \
                -skip tools \
                -skip translations \
                -skip webkit \
                -skip webkit-examples \
                -skip qtscript
