# AprilTagGenerator (SVG Vectorgraph) 

An AprilTag based Camera Calibration Board Vectorgraph Pattern Generator Based on Qt 

![3x3_demo](./3x3_demo.png)

download [3x3_demo.svg](./3x3_demo.svg)

## Prerequest

install apriltags cpp library (originally from [MIT](http://people.csail.mit.edu/kaess/apriltags/)): [huangqinjin/apriltags](https://github.com/huangqinjin/apriltags) or [wangdongustc/apriltags](https://github.com/wangdongustc/apriltags)

you might need [OpenCV](https://github.com/opencv/opencv/) when building the apriltags library

also Qt5 Widgets and Svg are needed to show the GUI and generate the SVG Vectorgraph Pattern

## Build and Running

run the following in your favorite shell, then things should work if no error happens: 
```
cd path/to/this/project
mkdir build
cd build 
cmake ..
make -j4
./AprilTagGenerator
```

## Notice
tags are arranged in a way that the start id comes in the bottom-left corner and the id increases from left to right and from rows to cols.

e.g. a 3x3 tag pattern starting from id 0 is arranged in the following way:
```
6 7 8
3 4 5
0 1 2
```

"Pixel"  means the smallest unit in the tag

"Border" means the most outer black border of each tag

"Corner Box" means the black boxes on the corner of the tags between them

"Padding" means the space between the edge of the entire painting and the tags

