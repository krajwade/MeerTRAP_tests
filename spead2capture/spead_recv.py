#!/usr/env/python

# A small script that capture SPEAD packets from the host and port


import numpy as np
import spead64_48 as spead
import logging
import sys


def receive():
    print 'RX: Initializing...'

    t = spead.TransportUDPrx(8888)
    ig = spead.ItemGroup()
    for heap in spead.iterheaps(t):
        #print spead.readable_heap(heap)
        ig.update(heap)
        print 'Got heap:', ig.heap_cnt
        for name in ig.keys():
          print '   ', name
          item = ig.get_item(name)
          print '      Description: ', item.description
          print '           Format: ', item.format
          print '            Shape: ', item.shape
          print '            Value: ', ig[name]
    print 'RX: Done.'


receive()
