# Ray-it

## Windows compilation

### Requirements:
- CMake
- Visual Studio C++

Make sure the Visual Studio compiler can be called from the console (cmd). Otherwise, open the x64 Native Tools Command Prompt for Visual Studio and compile from there.

### Instructions:
Go to the Ray-it folder and simply type
```Shell
compile_release.bat
```

## Linux compilation

### Requirements:
- CMake
- C++ compiler like gcc/g++ or equivalent (Clang)

### Instructions:
Go to the Ray-it folder and simply type
```Shell
sh linux_compile_release.sh
```


## Run a case
### Docker
#### Build
`docker build -t ray-it:latest .`
#### Run
`docker run -it ray-it`

in the docker terminal:

`./Ray-it_Release tests`