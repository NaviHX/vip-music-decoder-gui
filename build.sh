mkdir build
cd ./build
qmake ..
make

mv vip-music-decoder ..
cd ..
rm -r build