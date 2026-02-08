# iflash
A very basic GUI flashing tool 

[How to compile](#how-to-compile)

[Pre-built binary for OSX](#pre-built-binary)
# How to compile
Copy-paste to clone git repo and compile (with clang++, use g++ if needs be:)
```
git clone https://github.com/teddybearsoftware/iflash.git
cd iflash
clang++ main.cpp -std=c++17 `wx-config --cxxflags --libs` -o iFlash
```
Note: this requires wxWidgets 3.2+ installed. I have tested myself with wxWidgets 3.3.1 (latest dev release at time of writing.)

Then just run:
```
sudo ./iFlash
```
#Pre-built binary
OS X user? Just get the latest binary from [the project's Releases page](https://github.com/teddybearsoftware/iflash/releases/)
