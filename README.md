# vupdate

Vupdate is a basic updater tool to be included alongside your main software. It provides ability to - 
* download update files from remote servers
* offer version controlling by computing hashes of all the files and download only the needed ones
* distribute the software to anyone without the hassle of updating it constantly

## Built with

The app uses [curl](https://github.com/curl/curl) library for making all the downloads and [openssl](https://github.com/openssl/openssl) for computing the hashes.

Other dependencies include [spdlog](https://github.com/gabime/spdlog), [indicators](https://github.com/p-ranav/indicators) and [json](https://github.com/nlohmann/json).

## Installation

For Windows users, builds are published. For Linux and Mac users, the installation procedure is a little different and it is mentioned in the [Releases](https://github.com/GhostVaibhav/vupdate/releases/latest) page itself.

## Development

The software is generated by CMake. You have to install [OpenSSL](https://github.com/openssl/openssl) on your system before even starting this process. For doing so, you can take a look at your distro's package manager and find the required library to be installed. For Windows users, I would suggest you to download the binaries and set the path variable manually to the main folder. After setting up [OpenSSL](https://github.com/openssl/openssl), the process first involves the generation of all libraries by supplying the `COMPILE` argument as `0`. The command for which is given below - 

```sh
cmake .. -DCOMPILE=0
```

> Note: If you want to generate the project in a mode other than `Debug`, you can always use the `-DCMAKE_BUILD_TYPE` flag to specify the type of build. Just remember to use it with both the dependencies and project.

After this, the project needs to be built for all the libraries to be installed.

After installing all the libraries, we can supply the `COMPILE` argument to be `1`, for generating the main programs (`vpack`, `vupdate` and `vlib`). The command for which is given below -

```sh
cmake .. -DCOMPILE=1
```

After this, you can build the project.

## Contributions

Contributions are more than welcome, a couple of ideas will be added in the [open issues](https://github.com/GhostVaibhav/vupdate/issues?q=is%3Aopen) which you could take on - if you start working on one of them, please add a comment there.

Please check the [contribution guidelines](https://github.com/GhostVaibhav/vupdate/blob/main/CONTRIBUTING.md) for further details regarding the formatting.

## License

Distributed under the GPLv3 license. See the full [license text](https://github.com/GhostVaibhav/vupdate/blob/main/LICENSE) for more information.

## Contact

Vaibhav Sharma - [ghostvaibhav.codes](https://ghostvaibhav.codes)
