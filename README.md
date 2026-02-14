# gdb
Making my own version of gdb for linux!

# deps
```bash
sudo apt-get update
# if cmake not installed
sudo apt install cmake
# install dependencies
sudo apt install cmake libelf-dev libzydis-dev zydis-tools libreadline-dev
```

# compile and run
Run
```bash
cmake .
cmake --build .
```
A new binary `./gdb` will be created, thats it!
