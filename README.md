
# quic-node

## quiche 

3e9e5883670705703a3e5e3d083f1b0640c8aafc # 2023-8-7

## 

/root/quic-client/build/quiche-node localhost/echo --port 4433


##
TARGETS="arm64-v8a armeabi-v7a x86_64 x86"


cmake . -Bbuild -DCMAKE_TOOLCHAIN_FILE=/root/android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=android-19 -DANDROID_ABI=arm64-v8a
cmake --build build -- -j 16
