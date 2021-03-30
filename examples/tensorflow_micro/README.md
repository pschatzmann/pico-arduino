
Microcontrollers are not suitable to develop and train machine learning models. The resources are just not sufficient for this, but it is possible to use and deploy pre-trained models!

I have added the [Tensorflow hallo world example](https://github.com/pschatzmann/pico-arduino/tree/main/examples/tensorflow_micro) to my __Pico-Arduino framework__.

 The solution basically constists of only 3 implementation files:

- __tf_hallo_world.cpp__ contains the Arduino sketch
- __tf_hallo_world_model.h__ contains the (binary) model.
- __CMakeLists.txt__ is used to download Tensorflow with the help of FetchContent and build the source file
