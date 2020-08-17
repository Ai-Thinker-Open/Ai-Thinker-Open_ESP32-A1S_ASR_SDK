#!/bin/sh

project_path=$(cd `dirname $0`; pwd)  #获取当前目录
project_name="${project_path##*/}"    #获取当前所在文件名称
echo -e ""
echo -e "当前文件路径：" $project_path

if [ $project_path != "/bin" ]; then
	#环境变量设置
	cd ../../

	project_path1=$(cd `dirname $0`; pwd) 
	export IDF_PATH=$project_path1/esp-idf

	cd examples/Smart_home_scene_AI/

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
else
        echo -e ""
        echo -e ""
        echo -e "错误：没有正确获取到文件当前路径，请换一个终端工具试试！！！"
        echo -e ""
        echo -e ""
fi
