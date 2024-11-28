#!/bin/bash

# Script to create a .deb package for the MeteoSwiss library

# Set variables
PACKAGE_NAME="meteoswiss"
VERSION=$(cat VERSION)
ARCHITECTURE="amd64"  # Change as appropriate (e.g., i386, armhf)
MAINTAINER="Mathieu Bourquenoud"
DESCRIPTION="MeteoSwiss API Library"
LICENSE="LGPLv3"

BUILD_DIR="build/release"
DEB_DIR="deb_pkg"
DEB_PACKAGE="${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"

# Build the release
make clean
make

# Clean up any previous deb package directory
rm -rf ${DEB_DIR}
mkdir -p ${DEB_DIR}/DEBIAN
mkdir -p ${DEB_DIR}/usr/lib
mkdir -p ${DEB_DIR}/usr/include/meteoswiss
mkdir -p ${DEB_DIR}/usr/lib/pkgconfig

# Copy files to the package directory
cp ${BUILD_DIR}/lib${PACKAGE_NAME}.a ${DEB_DIR}/usr/lib/
cp ${BUILD_DIR}/lib${PACKAGE_NAME}.so ${DEB_DIR}/usr/lib/
cp includes/*.h ${DEB_DIR}/usr/include/meteoswiss/
cp ${BUILD_DIR}/${PACKAGE_NAME}.pc ${DEB_DIR}/usr/lib/pkgconfig/

# Create control file
cat <<EOF > ${DEB_DIR}/DEBIAN/control
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Section: libs
Priority: optional
Architecture: ${ARCHITECTURE}
Maintainer: ${MAINTAINER}
Description: ${DESCRIPTION}
License: ${LICENSE}
EOF

# Build the .deb package
dpkg-deb --build ${DEB_DIR} ${DEB_PACKAGE}

# Clean up
rm -rf ${DEB_DIR}

echo "Debian package created: ${DEB_PACKAGE}"
