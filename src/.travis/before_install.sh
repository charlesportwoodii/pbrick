#!/bin/bash

if [ -z "$(ls -A $HOME/gcc-arm-none-eabi-9-2019-q4-major)" ]; then
    cd $HOME
    wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2019q4/RC2.1/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
    tar -xf gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
    rm -rf gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
else
    echo "gcc-arm-none-eabi-9-2019-q4-major is already installed."
fi

if [ -z "$(ls -A $HOME/nrf_sdk/16.0.0)" ]; then
    cd $HOME
    mkdir -p $HOME/nrf_sdk/16.0.0
    wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5SDK160098a08e2.zip -O nRF5_SDK_16.0.0_98a08e2.zip
    mv nRF5_SDK_16.0.0_98a08e2.zip $HOME/nrf_sdk/16.0.0
    cd $HOME/nrf_sdk/16.0.0
    unzip nRF5_SDK_16.0.0_98a08e2.zip > /dev/null 2>&1
    rm -rf nRF5_SDK_16.0.0_98a08e2.zip
    pwd
    ls -laht $HOME/nrf_sdk/16.0.0
else
    echo "nRF5 SDK is already installed."
fi