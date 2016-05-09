#! /usr/bin/env python
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy

#vision_output_array = numpy.loadtxt('output_vision',delimiter=',')
vision_output_array = numpy.loadtxt('output_v',delimiter=',')

plt.plot(vision_output_array[:,0],label="1")
plt.plot(vision_output_array[:,1],label="2")
plt.plot(vision_output_array[:,2],label="3")
plt.plot(vision_output_array[:,3],label="4")
plt.legend()
plt.show()
