# SimpleCV

Some simple tools/libraries/demos for OpenCV3, integrated with Qt and OpenNI

### Dev-Env

The codes in this repo are developed and tested on _Ubuntu 16.04_

### Usage

**1. Building OpenCV from source code**:  
Use the automatic building script (in tools/build_cv)  
The script will generate the dev-env as follows:
 * The source code of opencv and opencv_contrib will be downloaded/updated in _opencv_git_ folder
 * The nesessary codes for building will be copied into the _sdk_ folder. The building works will be executed in this folder without making any change to the original source repos.
 * The binary distribution of the building works will be installed in _sdk/opencv_release_ folder.

When installing dependencies and python packages, an http proxy could be set at the beginning of the script (as a variable named _HTTP_PROXY_PROP_)

_TBC..._

## License

This project is licensed under the MIT License - see the
[LICENSE.md](LICENSE.md) file for details
