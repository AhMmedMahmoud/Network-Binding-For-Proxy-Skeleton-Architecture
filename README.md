# studied topics
- Sockets tcp/udp
- epoll of linux
- SOMEIP
- SOMEIP/SD
- Rpcs
- Events
- Service Discovery Process
- 370 slides based on
    - Explanation of ara::com API                                         
    - Explanation of Adaptive Platform Design                          
    - Explanation of Adaptive Platform Software Architecture 
    - SOME/IP Protocol Specification
    - SOME/IP Service Discovery Protocol Specification
- OOP in C++
    - Threads
    - Mutexs
    - Conditional variable
    - Promise future pattern
- Cmake
- WSL, VScode

# build instructions
```cmake
if [ -d "build" ]; then
    rm -r build
fi

mkdir build
cd build
cmake ..
make
```
