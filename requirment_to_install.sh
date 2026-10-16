#!/bin/sh

apt-get update && apt-get install -y \
    make \
    nasm \
    gcc-multilib \
    grub-common \
    grub-pc-bin \
    xorriso