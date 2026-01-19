#!/bin/bash

# Install dependencies
sudo apt update
sudo apt install -y build-essential autoconf libtool pkg-config cmake git clangd

# Initialize and update git submodules
git submodule update --init --recursive --jobs $(nproc)

pushd libs

# Checkout specific versions for reproducibility
pushd spdlog
git checkout tags/v1.14.1
popd

popd

echo ""
echo "Setup complete!"
echo "To build without tracing: cd build && cmake .. && make -j\$(nproc)"
echo "To build with tracing:    cd build && cmake -DTRACING=ON .. && make -j\$(nproc)"
