#!/bin/bash

git clone https://github.com/s3team/Intel-Pin-Archive.git
git clone https://github.com/s3team/Abacus.git --branch old


cd Abacus
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..; make -j4; cp QIF ../../QIF-old
cd ../../
mkdir build
cd build
cmake ..
make -j4
cp ./App/QIF/QIF ../QIF-new

cd /abacus/Pintools/CryptoLibrary 
make TARGET=ia32 PIN_ROOT=/abacus/Intel-Pin-Archive -j4
cd /abacus

