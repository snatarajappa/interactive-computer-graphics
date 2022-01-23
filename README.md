# CS 6610 - Spring 2022 Interactive Computer Graphics
This repository contains the projects for graduate interactive computer graphics course.

Course URL: https://graphics.cs.utah.edu/courses/cs6610/spring2022/

## Build Environment

OS: OS X 12.01

IDE: XCode 13.2.1

Libararies and dependencies:
* C++
* OpenGL
* freeglut

## Steps to compile and run the projects

Run the below command in the project directory. 

```curl
clang++  -arch x86_64 -o run {project_name}/main.cpp lib/libglut.3.11.0.dylib -framework OpenGL -I include -L lib
```
Run the below command to run the executable.
```curl
./run
```
