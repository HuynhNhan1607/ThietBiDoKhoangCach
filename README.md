| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# _Sample project_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is the simplest buildable example. The example is used by command `idf.py create-project`
that copies the project to user specified path and set it's name. For more information follow the [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project)



## How to use example
We encourage the users to use the example as a template for the new projects.
A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

## Example folder contents

The project **sample_project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   ├── idf_component.yml
│   ├── main.cpp
│   │
│   ├── lib
│   │   ├── gpio_handler.h
│   │   ├── http_server.h
│   │   ├── nvs_handle.h
│   │   ├── vl53l0x_handler.h
│   │   ├── WebSocket.h
│   │   └── wifi_handler.h
│   │
│   ├── pololu_vl53l0x
│   │   ├── i2c.cpp
│   │   ├── i2c.h
│   │   ├── vl53l0x.cpp
│   │   └── vl53l0x.h
│   │
│   ├── src
│   │   ├── gpio_handler.c
│   │   ├── http_server.cpp
│   │   ├── nvs_handle.c
│   │   ├── vl53l0x_handler.cpp
│   │   ├── WebSocket.c
│   │   └── wifi_handler.c
│   │
│   └── webserver
│       ├── index.html
│       ├── script.js
│       └── style.css
│
└── README.md                  This is the file you are currently reading

```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
