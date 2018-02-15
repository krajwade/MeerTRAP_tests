#!/usr/env/python
# Small script to send spead2 packets on the localhost (for now) and receive on the other side using a receive script

# Kaustubh Rajwade

import numpy as np
import spead64_48 as spead
import logging
import sys
import datetime


# Setup the transfer

nchans = 4096
ntime = 64
nsamples = 128
nbeams = 6

print 'TX: Initializing...'

# send 6 beams and full band
for i in range(nbeams):

  tx = spead.Transmitter(spead.TransportUDPtx('127.0.0.1', 8888, rate=1e3))

# Send 100 heaps
  for j in range(100):

    ig = spead.ItemGroup()

  # Timestamp

    time = str(datetime.datetime.now().time())


    ig.add_item(name='Timestamp', description='Current time',
        shape=[], fmt=spead.mkfmt(('c',8)),
                init_val=('00:00'))

    ig['Timestamp'] = time

# Beam ID
    ig.add_item(name='beam id', description='beam number',
          shape=[], fmt=spead.mkfmt(('u',32)),
                          init_val=(0))
    ig['beam id'] = i


# frequency


    ig.add_item(name='frequency', description='frequency channel number of the first channel',
            shape=[], fmt=spead.mkfmt(('u',32)),
                                      init_val=(0))
    ig['frequency'] = 4096


# data

    data = np.arange(4096*64*128).astype(np.uint32); data.shape = (4096,64,128)
    ig.add_item(name='data', description='Beamformed data for the given beam id',
              shape=[4096,64,128], ndarray=data)
    ig['data'] = data

# send the heap

    tx.send_heap(ig.get_heap())

  tx.end()

# Done!

print "trasmission done!"
