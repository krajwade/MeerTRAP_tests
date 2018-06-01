#!/usr/env/python
# Small script to send spead2 packets on the localhost (for now) and receive on the other side using a receive script

# Kaustubh Rajwade

import numpy as np
import spead64_48 as spead
import logging
import sys
import datetime


# Set up packet transfer based on arguments

if (len(sys.argv) != 7):
    print("Incorrect number of arguments")
    print ("Correct Usage: python spead2_send.py <nheaps> <nchans> <ntime> <nsamples> <nfreq> <nbeams>")
    exit()

# Setup the transfer

nchans = int(sys.argv[2])
ntime = int(sys.argv[3])
nsamples = int(sys.argv[4])
nfreq = int(sys.argv[5])
nbeams = int(sys.argv[6])

print ('TX: Initializing...')

ind = 0
# Send nbeams beams

for i in range(nbeams):

  tx = spead.Transmitter(spead.TransportUDPtx('127.0.0.1', 8888, rate=1e3))

# Send N heaps
  for j in range(int(sys.argv[1])):

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
    # cycle through the frequency channels in steps of 128, 32 times
    
    ig['frequency'] = 128*ind
    if (ind == 32):
     ind = 0
    else:
     ind +=1

#nchans

    ig.add_item(name='nchans', description='Number of channels per heap',
            shape=[], fmt=spead.mkfmt(('u',32)),
                                       init_val=(0))
    ig['nchans'] = 128

# data

    data = np.arange(128*64*128).astype(np.uint32); data.shape = (128,64,128)
    ig.add_item(name='data', description='Beamformed data for the given beam id',
              shape=[128,64,128], ndarray=data)
    ig['data'] = data

# send the heap

    tx.send_heap(ig.get_heap())

  tx.end()

# Done!

print ("trasmission done!")
