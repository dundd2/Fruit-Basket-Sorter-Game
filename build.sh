#!/bin/bash

# Cleanup previous build artifacts
rm -rf build
rm -f fruity-game

# Create and enter build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make -j$(nproc)

# Create DEB package
cpack

# Copy files to parent directory
cp fruity-game ..
if [ -f *.deb ]; then
    cp *.deb ..
fi

# Cleanup build directory except debian package
find . -type f ! -name "*.deb" -delete

echo "Build completed! You can run the game with ./fruity-game"
