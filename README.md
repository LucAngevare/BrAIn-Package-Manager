# BrAIn Package Manager - !Work in progress!

This is definitely a project I want to finish in the future but I just don't have the time, documentation nor the resources to finish it in it's current state. While the uWebSockets.js documentation is full and complete, the C++ version (the version that was actually first) basically doesn't have any proper documentation. I might be the only one who shares this opinion but so be it. Once the server is done I need to make an API and add a public view so people can actually see what packages there are along with documentation.

[![LucAngevare - BrAIn-Package-Manager](https://img.shields.io/static/v1?label=LucAngevare&message=BrAIn-Package-Manager&color=blue&logo=github)](https://github.com/LucAngevare/BrAIn-Package-Manager)
[![stars - BrAIn-Package-Manager](https://img.shields.io/github/stars/LucAngevare/BrAIn-Package-Manager?style=social)](https://github.com/LucAngevare/BrAIn-Package-Manager)
[![forks - BrAIn-Package-Manager](https://img.shields.io/github/forks/LucAngevare/BrAIn-Package-Manager?style=social)](https://github.com/LucAngevare/BrAIn-Package-Manager)

This is the official repository for BPM, BrAIn Package Manager. This is the server for the package manager for BrAIn API. Using this you can download and use add-ons on your API. The official package manager is on IP:port (will host it when it's on the first major version).

### Table of contents
- [BrAIn Package Manager](#brain-package-manager)
    - [Table of contents](#table-of-contents)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Ideas for the future](#ideas-for-the-future)
  - [Contributing](#contributing)

## Installation

Download the header-only file of [nlohmann's JSON](https://github.com/nlohmann/json) and put that into a folder called "nlohmann" in your project folder. Then download all files on pcrecpp (no idea if I'm going to keep that, if I don't, I'll update this README) and add that to the folder named "pcre". Lastly, do the same for [pugixml](https://github.com/zeux/pugixml). Then install, compile and run [uWebSockets](https://github.com/uNetworking/uWebSockets/) and put that in the parent directory.

Compile main.cpp with the command:
```
g++ main.cpp -o main -pthread -lpthread -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++2a -I../uWebSockets/src -I../uSockets/src -I../uWebSockets/libdeflate -DUWS_USE_LIBDEFLATE ../uSockets/*.o -lpcrecpp -lz ../uWebSockets/libdeflate/libdeflate.a
```
and you're able to run the executable by just running `./main` after that.

## Usage

Do the above to install the libraries, and run the executable. You are able to test the package manager with

- `curl -v http://127.0.0.1:3000/get/Designer`,


- `python3 test.py` or


- `curl -v -F ZIP=@/path/to/any/ZIP http://127.0.0.1:3000/give`

## Ideas for the future

A lot is not done currently, and a heck of a lot of bugs are currently in place (corrupted Base64 data, unparseble formdata, PugiXML only currently giving 1 output, etc). If I'm able to fix all of the bugs, I will test all the vulnerabilities I personally can and release it together with the BrAIn API.

## Contributing

Any help is always appreciated, if you have any good ideas, feel free to open an issue or discussion, if you have any great code to improve mine, feel free to open a pull request.
