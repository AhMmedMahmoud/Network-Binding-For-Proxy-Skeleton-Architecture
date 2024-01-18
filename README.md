# studied topics
- SOMEIP
- SOMEIP/SD
- Sockets tcp/udp
- epoll of linux
- Events
- Rpcs
- Service Discovery Process
- Threads
- Mutexs
- Conditional variable
- Promise future pattern
- OOP in C++
- Cmake
- 370 slides based on
    - Explanation of ara::com API                                         
    - Explanation of Adaptive Platform Design                          
    - Explanation of Adaptive Platform Software Architecture 
    - SOME/IP Protocol Specification
    - SOME/IP Service Discovery Protocol Specification
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
