# OpenCV拾趣（一）——构建自己的OpenCV SDK
## 本篇简介
谈起一套工具的使用，首先需要的就是这套工具的安装。因为这个系列的研究所使用的开发系统为Ubuntu16.04，而官方目前没有提供3.3版本的安装包，因此需要自己从源码进行编译。

既然都已经必须从源码编译了，不妨添加一些基础安装包里没有包含的高级模块，打造一个功能更为全面的SDK。

## 编写构建脚本
在从源码构建一套工具时，因为系统环境差异等各方面的因素，往往会出现一些预料之外的情况导致构建失败，很少能一次就构建成功的情况，因此我个人习惯编写简单的构建脚本来进行构建，减少构建失败时重复敲打命令的工作。

首先规划一下构建环境的目录结构：  
>simpleCV  
>   |  
>   +-->tools:工具文件夹  
>   |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|  
>   |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+-->build_cv：构建脚本  
>   |    
>   +-->opencv_git：官方代码库文件夹  
>   |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|  
>   |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+-->opencv：官方opencv基础库源码  
>   |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|  
>   |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+-->opencv_contrib：官方contrib源码  
>   |  
>   +-->sdk  
>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|  
>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+-->opencv：构建目录，避免污染源码库环境  
>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|  
>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+-->opencv_release：发布目录，也就是最终使用的sdk  

然后大致划分一下构建的步骤：  
1. 从官方的git repo里拉取最新的源码
2. 安装依赖
3. 准备构建环境
4. 执行cmake和make进行构建
5. 部署其他库文件

接下来开始编写脚本。先编写一个基本的脚本框架：
```bash
#!/bin/bash
# 全局变量
ROOT_DIR=$(pwd)/..
GIT_DIR=${ROOT_DIR}/opencv_git
SDK_DIR=${ROOT_DIR}/sdk
BUILD_DIR=${SDK_DIR}/opencv
RELEASE_DIR=${SDK_DIR}/opencv_release
VERSION=3.3.1
HTTP_PROXY_PROP=127.0.0.1:8123

# 执行入口
#update_opencv_src &&
#	install_dependencies &&
#	prepare_src_codes &&
#	exec_cmake &&
#	exec_make &&
#	deploy_extra_libs
```
其中：
* 全局变量：定义了前面规划的目录结构、准备编译的opencv版本和代理设置
* 执行入口部分：按照上面划分的步骤，把每一步都包成一个函数，编译的时候如果出错了，可以把前面已经成功的步骤注释掉，只调试出问题的步骤。（现在因为函数都还没有编写，所以暂时先注释起来。）

下面就来编写各个步骤的函数：
### 1. 拉取最新源码
目前官方的最新代码分为基础包和contrib包，其中contrib包囊括了一些技术较新，但是因为各种原因还没有集成在默认OpenCV中的功能。[[1](#ref1)]

这两个包分别对应官方github的opencv和opencv_contrib这两个repo，且这两个repo的版本号是基本一致的。例如本系列使用的3.3。1版本opencv，只需要将两个库均切换到3.3.1标签即可。

下面是拉取最新源码的脚本：
```bash
update_opencv_src() {
	echo "Updating soruce code..."
	local src_dirs=(${GIT_DIR} ${SDK_DIR})
	ensure_dirs ${src_dirs[@]}
	cd ${GIT_DIR}
	if [[ -d ${GIT_DIR}/opencv/.git ]]; then
		git pull
	else
		git clone https://github.com/opencv/opencv.git
	fi
	if [[ -d ${GIT_DIR}/opencv_contrib ]]; then
		git pull
	else
		git clone https://github.com/opencv/opencv_contrib
	fi
	cd ${GIT_DIR}/opencv
	git checkout ${VERSION}
	cd ${GIT_DIR}/opencv_contrib
	git checkout ${VERSION}
}
```
这里面用到了一个确保关键目录存在的工具函数
```bash
ensure_dirs() {
	local targets=$@
	for target in ${targets[@]}; do
		echo -n "checking ${target}..."
		if [[ ! -d ${target} ]]; then
			mkdir -p ${target}
		fi
		echo "OK!"
	done
}
```
### 2.安装依赖
3.3.1需要安装的依赖参见下面的安装依赖函数。值得注意的是有些依赖包可能会安装得比较缓慢，有条件的话可以设置一下代理。下面的脚本里也包含了对代理设置的判断：
```bash
install_dependencies() {
	echo "Installing dependencies..."
	if [[ -n ${HTTP_PROXY_PROP} ]]; then
		export http_proxy=http://${HTTP_PROXY_PROP}
		export https_proxy=https://${HTTP_PROXY_PROP}
	fi

	sudo apt install -y libjpeg8-dev libjasper-dev libpng12-dev &&
		sudo apt install -y libtiff5-dev &&
		sudo apt install -y libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev &&
		sudo apt install -y libxine2-dev libv4l-dev &&
		sudo apt install -y libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev &&
		sudo apt install -y qt5-default libgtk2.0-dev libtbb-dev &&
		sudo apt install -y libatlas-base-dev &&
		sudo apt install -y libfaac-dev libmp3lame-dev libtheora-dev &&
		sudo apt install -y libvorbis-dev libxvidcore-dev &&
		sudo apt install -y libopencore-amrnb-dev libopencore-amrwb-dev &&
		sudo apt install -y x264 v4l-utils &&
		sudo apt install -y python-dev python-pip python3-dev python3-pip

	if [[ -n ${HTTP_PROXY_PROP} ]]; then
		sudo -H pip2 --proxy ${HTTP_PROXY_PROP} install -U pip numpy &&
			sudo -H pip3 --proxy ${HTTP_PROXY_PROP} install -U pip numpy
	else
		sudo -H pip2 install -U pip numpy &&
			sudo -H pip3 install -U pip numpy
	fi
}
```
### 3. 准备构建环境
为了避免污染源码库，可以单独搭建一个构建环境，将两个源码库中的源码合并在一起，然后在新搭建的构建环境中进行构建：
```bash
prepare_src_codes() {
	echo "Preparing build environment..."
	if [[ -d ${BUILD_DIR} ]]; then
		rm -rf ${BUILD_DIR}
	fi
	mkdir ${BUILD_DIR}
	cd ${GIT_DIR}/opencv
	local src_modules=(3rdparty apps cmake data doc include modules platforms samples)
	for module in ${src_modules[@]}; do
		echo "copying opencv/${module}..."
		cp -Rf ${module} ${BUILD_DIR}/${module}
	done

	cp -Rf CMakeLists.txt ${BUILD_DIR}
	cp -Rf LICENSE ${BUILD_DIR}

	if [[ -d ${BUILD_DIR}/contrib ]]; then
		rm -rf ${BUILD_DIR}/contrib
	fi
	mkdir ${BUILD_DIR}/contrib
	cd ${GIT_DIR}/opencv_contrib
	local contrib_modules=(doc modules samples)
	for module in ${contrib_modules[@]}; do
		echo "copying opencv_contrib/${module}..."
		cp -Rf ${module} ${BUILD_DIR}/contrib/${module}/
	done
}
```

### 4. 执行cmake
准备好构建环境后，就可以尝试进行编译了。官方的源码已经提供了CMakeList.txt，所以最简单的办法就是直接使用cmake作为编译工具。

这里需要特别注意两点：
1. **请保持网络畅通**。官方设置的编译过程中会下载一些所需的依赖库，如果下载失败导致cmake执行失败，可以尝试多执行几次。
2. **请确保系统中的cmake包含https支持。**这里面有个比较坑的地方，就是一些重要的依赖库需要使用https下载，而下载失败cmake不会报错，会跳过继续执行，但是在后面执行make的时候会报一些诸如找不到cuda.hpp之类的诡异错误。如果发现所使用的cmake不包含https支持，可以重新编译一下cmake。具体可以参考下面的[参考链接3](https://github.com/opencv/opencv_contrib/issues/1131)

——综合上面两点，还是和前面安装依赖包时一样，有条件的话推荐在这一步使用一下代理。

言归正传，官方的CMakeList中提供了非常多的选项，可以通过在cmake后添加-D标签进行控制，下面的脚本仅挑选了我认为有用的模块，如果有其他进一步的需要可以参考CMakeList中的设置进行调整。

为了方便调试，这个函数还添加了一个--clean的参数来清理构建目录，如果需要的话在执行入口的exec_cmake后面加上--clean即可：
```bash
exec_cmake() {
	echo "Executing cmake..."
	cd ${BUILD_DIR}
	if [[ -d build ]]; then
		if [[ $1 -eq "--clean" ]]; then
			rm -rf build/
		fi
	else
		mkdir build
	fi
	cd build
	cmake -D CMAKE_BUILD_TYPE=RELEASE \
		-D CMAKE_INSTALL_PREFIX=${RELEASE_DIR} \
		-D INSTALL_C_EXAMPLES=ON \
		-D INSTALL_PYTHON_EXAMPLES=ON \
		-D WITH_TDD=ON \
		-D WITH_V4L=ON \
		-D WITH_QT=ON \
		-D WITH_OPENGL=ON \
		-D OPENCV_EXTRA_MODULES_PATH=${BUILD_DIR}/contrib/modules \
		-D BUILD_EXAMPLES=ON ..
}
```

### 5. 执行make
执行到这一步基本上就没什么问题了。需要注意的就是上面一步所说的，如果cmake的https支持缺失的话，可能会报一些奇怪的错，如果构建执行make的时候报了什么错，请检查一下所使用的cmake。
```bash
exec_make() {
	"Making..."
	cd ${BUILD_DIR}/build
	local core_cnt=$(nproc)
	make -j${core_cnt} && make install
}
```

### 6. 部署其他库文件
在使用编译好的sdk时，根据cmake设置的不同，可能会有一些动态库需要额外部署到sdk中以方便链接时使用。下面是部署libippicv到sdk路径下的脚本：
```bash
deploy_extra_libs() {
	#libippicv
	echo -n "Deploying libippicv..."
	local lib_ippicv_path=$(find ${BUILD_DIR}/build/3rdparty/ippicv -name "libippicv.a")
	if [[ -n ${lib_ippicv_path} ]];then
		cp ${lib_ippicv_path} ${RELEASE_DIR}/lib && echo "OK" || echo "FAILED!"
	else
		echo "FAILED！libippicv not found!"
	fi
	
}
```

至此，sdk就构建完成了。（[点击这里查看完整脚本](https://github.com/KevinEmiya/simpleCV/blob/master/tools/build_cv)）

下一步的尝试是使用这个sdk结合Qt进行开发。具体请参见下一节。

## 参考链接
1. [OpenCV 3 - 编译更强大的OpenCV](http://blog.csdn.net/github_32886825/article/details/53303922)：windows环境下拓展和编译opencv的系列教程
2. [Install OpenCV on Ubuntu](https://www.learnopencv.com/install-opencv3-on-ubuntu/)：Ubuntu上安装opencv的教程，列举了所需的依赖包
3. [CMake缺失https下载支持导致的错误讨论](https://github.com/opencv/opencv_contrib/issues/1131)