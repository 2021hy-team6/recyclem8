# recyclem8
C++ Application for the Recycling Assistant Project

#### Tested Environments
- Ubuntu 20.04 LTS
- Qt 5.15.2
- GCC 5.3.1
  - Tested on a R7 5800x Quad Core VM, and a Dual Core Laptop

#### Requirements
> ##### In order to adhere to library licenses, this project will not provide a pre-compiled statically linked binary.
- Qt: `sudo apt install qtbase5-dev`
- fmt: https://github.com/fmtlib/fmt
- OpenCV 4 Core: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
- PostgreSQL 12: `sudo apt install postgresql-server-dev-all` `sudo apt install postgresql-12`
- libpq: `sudo apt install libpq-dev`
- libpqxx: https://github.com/jtv/libpqxx

Despite the following being provided as a submodule, you may also need the following:
- rapidjson: https://github.com/Tencent/rapidjson

Note: If OpenCV4 is not building with FFMpeg, you may require the following:
`sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev`

#### Compiling/Installing
```
git clone https://github.com/2021hy-team6/recyclem8.git --recursive
cd ./recyclem8
mkdir build && cd build
cmake ..
make -j
```

Copy & paste the contents of the assets folder into the build directory.

It should look like the following:
```
build
└ recycleAssistant
└ classifications.json
└ config.json
└ frozen_taco.pb
└ frozen_taco.pbtxt
└ (other build files)
```

#### Usage Guide
Launching the application is straightforward: `./recycleAssistant`

---

In the Settings Context Menu (top left), you will have to set up your video source, whether it is connected physically, or through a webcam. After the value is set, you may close out of the settings window, and use the "Reconnect to Camera" action in the Settings Context Menu.

You may choose to enable usage of the database, which pairs with the recycling dashboard here: https://github.com/2021hy-team6/recycle-assistant
In order for this to work successfully, it requires that the database has been started, and the correct database connection string is provided - which it should be, by default.

You can also choose to skip the start sequence, which will skip directly to the scanning screen on application start, rather than go through the starting screen.

Session statistics can be viewed in the Statistics window, located in the top left Statistics context menu, and in the "View Statistics" action. If you do set up the dashboard, and the database, it is recommended to go into the config.json file and manually add the generated dashboard_link into the "dashboard_link" key, so that it is directly accessible through the statistic windows' dashboard button.
