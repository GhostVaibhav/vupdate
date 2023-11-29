export BUILD_TYPE='Release'

mkdir build && cd build
cmake .. -DCOMPILE=0 -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . -j $(nproc) --config $BUILD_TYPE

cmake .. -DCOMPILE=1 -DCMAKE_INSTALL_PREFIX=/usr -DDO_NOT_FORMAT=OFF -DCMAKE_BUILD_TYPE=$BUILD_TYPE
DESTDIR=AppDir cmake --build . -j $(nproc) --config $BUILD_TYPE --target install
