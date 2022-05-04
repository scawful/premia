# Premia

- Platform: Windows, macOS, GNU/Linux
- Dependencies: Boost, SDL2, ImGui, ImPlot, libCURL, OpenSSL

## Description
Premia is a general purpose financial market analysis and trading program. Currently, the program supports linking a TDAmeritrade API key to gain access to features like account information, watchlists, positions, and (soon) trading actions. 

#### Supported Services: 
- TDAmeritrade
- CoinbasePro
- IEXCloud 

More details about the future of Premia can be found on my website [halext.org/labs/Premia](https://halext.org/labs/Premia/)

Building and installation
-------------------------
[CMake](http://www.cmake.org "CMake") is required to build Premia 

1. Clone the repository

        git clone https://github.com/scawful/premia.git
 
2. Move to the libs directory and clone both [ImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot) as they are built static.

        git clone https://github.com/ocornut/imgui.git
        git clone https://github.com/epezent/implot.git

3. Create a build directory in the root workspace folder

        mkdir build
      
4. Move to the build directory and generate the build files specified in CMakeLists.txt

        cmake -G "<MinGW Makefiles/Unix Makefiles>" ../

5. Build and run.

        make Premia
        cmake --build <premia_root/build> --config Debug --target Premia

API Key Instructions
----------

For TDAmeritrade users to link their account, the program starts with a login window that takes as input your consumer key and refresh token.

Alternatively, you can create an apikey.txt and place it in the assets/ folder with your consumer key on the first line and the refresh token you generated on the second line for Premia to load those values for you. 

Screenshots
--------
![](https://i.ibb.co/99SsWWb/Screen-Shot-2022-03-06-at-3-46-48-PM.png)
<!-- ![](https://i.ibb.co/X8cSy1T/C15-E0-A46-8357-4-D8-A-BD9-F-15-CDA776-AAF1.png)
![](https://i.ibb.co/NjdT3GS/49-C38803-E56-D-4648-9-F9-F-704-E661-C0-D7-C.png)
![](https://i.ibb.co/JHD6MFK/Screen-Shot-2022-01-09-at-9-52-33-AM.png) -->
