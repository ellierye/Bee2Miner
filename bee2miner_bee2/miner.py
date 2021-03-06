#!/usr/bin/env python
# by teknohog
# Python wrapper for Xilinx Serial Miner
# Modified for use with bee2miner
# Opens socket instead of serial connection

#rate at which we ask the server for new work
# (aka giving up on the block we were working on).
# Our implementation is running at 12.5 kHash/sec,
# so 2^32 nonce attempts takes 5.7 minutes.
# Since that's half a friggin block time I'll make it 3 minutes.
# Actually apparently a 60 second limit is more
# reasonable (since transactions are being added to the blocks
# at about that rate).
askrate = (1 * 60)

###############################################################################

import socket
import sys

from optparse import OptionParser
from jsonrpc import ServiceProxy
from time import ctime, sleep, time
from threading import Thread, Event
from Queue import Queue

# Options
parser = OptionParser()
parser.add_option("-u", "--user", dest="user", help="username")
parser.add_option("-w", "--pass", dest="password", help="password")
parser.add_option("-t", "--host", dest="host", help="host",
                  default="uswest.btcguild.com")
parser.add_option("-p", "--port", dest="port", help="port", type="int",
                  default=8332)
parser.add_option("-x", "--minerhost", dest="minerhost",
                  help="miner host address")
parser.add_option("-y", "--minerport", dest="minerport", type="int",
                  help="miner port")
(options, args) = parser.parse_args()

if len(options.user) == 0:
    print "Need username."

if len(options.password) == 0:
    print "Need password."
# end options

def stats(count, starttime):
    # 2**32 hashes per share (difficulty 1)
    mhshare = 4294.967296

    s = sum(count)
    tdelta = time() - starttime
    rate = s * mhshare / tdelta

    # This is only a rough estimate of the true hash rate,
    # particularly when the number of events is low. However, since
    # the events follow a Poisson distribution, we can estimate the
    # standard deviation (sqrt(n) for n events). Thus we get some idea
    # on how rough an estimate this is.

    # s should always be positive when this function is called, but
    # checking for robustness anyway
    if s > 0:
        stddev = rate / s**0.5
    else:
        stddev = 0

    return "[%i accepted, %i failed, %.2f +/- %.2f Mhash/s]" % (count[0], count[1], rate, stddev)

class Reader(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.daemon = True

    def run(self):
        while True:
            nonce = sock.recv(4)

            if len(nonce) == 4:
                # Keep this order, because writer.block will be
                # updated due to the golden event.
                submitter = Submitter(writer.block, nonce)
                submitter.start()
                golden.set()


class Writer(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.daemon = True

        # This is accessed by other threads, so keep something
        # sensible available
        self.block = "0" * 256

    def run(self):
        while True:
            try:
                work = bitcoin.getwork()
            except:
                print("RPC getwork error")
                # In this case, keep crunching with the old data. It will get 
                # stale at some point, but it's better than doing nothing.
                
            self.block = work['data']
            
            midstate = work['midstate']

            # Just a reminder of how Python slices work in reverse
            #rdata = self.block.decode('hex')[::-1]
            #rdata2 = rdata[32:64]
            rdata2 = self.block.decode('hex')[95:63:-1]
            rmid = midstate.decode('hex')[::-1]
            
            payload = rmid + rdata2
            
            sock.send(payload)
            #print("Sending new work...\n");

            #Note: My version of python is 2.5, which
            # is old and lousy and does not
            # actually return something meaningful
            # (That change happened in python 2.7).
            # So have to check the flag after wait returns.
            # result = golden.wait(askrate)
            # Not really sure why we wouldn't clear the
            # event unconditionally.
            golden.wait(askrate);
            
            #if (result):
            golden.clear()

class Submitter(Thread):
    def __init__(self, block, nonce):
        Thread.__init__(self)

        self.block = block
        self.nonce = nonce

    def run(self):
        # This thread will be created upon every submit, as they may
        # come in sooner than the submits finish.

        print("Block found on " + ctime())

        hrnonce = self.nonce[::-1].encode('hex')
        data = self.block[:152] + hrnonce + self.block[160:]

        try:
            result = bitcoin.getwork(data)
            print("Upstream result: " + str(result))
        except:
            print("RPC send error")
            # a sensible boolean for stats
            result = False

        results_queue.put(result)

class Display_stats(Thread):
    def __init__(self):
        Thread.__init__(self)

        self.count = [0, 0]
        self.starttime = time()
        self.daemon = True

    def run(self):
        while True:
            result = results_queue.get()
            
            if result:
                self.count[0] += 1
            else:
                self.count[1] += 1
                
            print(stats(self.count, self.starttime))
                
            results_queue.task_done()

golden = Event()

url = 'http://' + options.user + ':' + options.password + '@' + options.host + ':' + str(options.port)

bitcoin = ServiceProxy(url)

results_queue = Queue()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((options.minerhost, options.minerport))

reader = Reader()
writer = Writer()
disp = Display_stats()

reader.start()
writer.start()
disp.start()

try:
    while True:
        # Threads are generally hard to interrupt. So they are left
        # running as daemons, and we do something simple here that can
        # be easily terminated to bring down the entire script.
        sleep(10000)
except KeyboardInterrupt:
    print("Terminated")

sock.close()
