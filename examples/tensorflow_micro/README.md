
Microcontrollers are not suitable to develop and train machine learning models. The resources are just not sufficient for this, but it is possible to use and deploy pre-trained models!

I have added a simplified __Tensorflow hallo world example__ where the model has been trained on the sin function, 
to my [Pico-Arduino framework](https://github.com/pschatzmann/pico-arduino).

The solution consists of only 3 implementation files:

- __tf_hallo_world.cpp__ contains the Arduino sketch
- __tf_hallo_world_model.h__ contains the (binary) trained model.
- __CMakeLists.txt__ is used to download Tensorflow with the help of FetchContent and build the source file

Further information can be found in the [official documentation...](https://www.tensorflow.org/lite/microcontrollers)
