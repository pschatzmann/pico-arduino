
The DMA is a cool functionality that we can use to copy data in the background while the processor is doning some other work.
Here is an [easy to use C++ API](https://pschatzmann.github.io/pico-arduino/doc/html/class_pico_d_m_a.html).

In this demo I just use the DMA to 

- initialize an array with some defined values and
- copy an array 

As we can see we manage to print around 8 chanracters on the screen while we copy the data in the background....