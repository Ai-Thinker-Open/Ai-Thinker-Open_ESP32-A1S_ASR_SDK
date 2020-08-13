
#环境变量设置
export IDF_PATH=~/test/Ai-Thinker-Open_ESP32-A1S_ASR_SDK/esp-idf


#clean
make clean

#配置
make menuconfig

#编译
make -j99

#擦写flash
make erase_flash

#烧入
make flash

#监听
make monitor
