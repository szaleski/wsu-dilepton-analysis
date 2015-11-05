#!/bin/bash

./runEndpoint.py -i maxbias005b1000pt50f1000_asym -o maxbias005b1000pt50f1000_asym_rebinned25.root -b0.005 -f1000 -n1000 -r25 >&endpoint.maxbias005b1000pt50f1000_asym_rebinned25.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_asym -o maxbias005b1000pt50f1000_asym_rebinned50.root -b0.005 -f1000 -n1000 -r50 >&endpoint.maxbias005b1000pt50f1000_asym_rebinned50.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_asym -o maxbias005b1000pt50f1000_asym_rebinned10.root -b0.005 -f1000 -n1000 -r10 >&endpoint.maxbias005b1000pt50f1000_asym_rebinned10.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_asym -o maxbias005b1000pt50f1000_asym_rebinned1.root  -b0.005 -f1000 -n1000 -r1  >&endpoint.maxbias005b1000pt50f1000_asym_rebinned1.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_asym -o maxbias005b1000pt50f1000_asym_rebinned5.root  -b0.005 -f1000 -n1000 -r5  >&endpoint.maxbias005b1000pt50f1000_asym_rebinned5.log&

./runEndpoint.py -i maxbias001b1000pt50f1000_asym -o maxbias001b1000pt50f1000_asym_rebinned25.root -b0.001 -f1000 -n1000 -r25 >&endpoint.maxbias001b1000pt50f1000_asym_rebinned25.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_asym -o maxbias001b1000pt50f1000_asym_rebinned50.root -b0.001 -f1000 -n1000 -r50 >&endpoint.maxbias001b1000pt50f1000_asym_rebinned50.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_asym -o maxbias001b1000pt50f1000_asym_rebinned10.root -b0.001 -f1000 -n1000 -r10 >&endpoint.maxbias001b1000pt50f1000_asym_rebinned10.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_asym -o maxbias001b1000pt50f1000_asym_rebinned1.root  -b0.001 -f1000 -n1000 -r1  >&endpoint.maxbias001b1000pt50f1000_asym_rebinned1.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_asym -o maxbias001b1000pt50f1000_asym_rebinned5.root  -b0.001 -f1000 -n1000 -r5  >&endpoint.maxbias001b1000pt50f1000_asym_rebinned5.log&

./runEndpoint.py -i maxbias0005b1000pt50f1000_asym -o maxbias0005b1000pt50f1000_asym_rebinned25.root -b0.0005 -f1000 -n1000 -r25 >&endpoint.maxbias0005b1000pt50f1000_asym_rebinned25.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_asym -o maxbias0005b1000pt50f1000_asym_rebinned50.root -b0.0005 -f1000 -n1000 -r50 >&endpoint.maxbias0005b1000pt50f1000_asym_rebinned50.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_asym -o maxbias0005b1000pt50f1000_asym_rebinned10.root -b0.0005 -f1000 -n1000 -r10 >&endpoint.maxbias0005b1000pt50f1000_asym_rebinned10.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_asym -o maxbias0005b1000pt50f1000_asym_rebinned1.root  -b0.0005 -f1000 -n1000 -r1  >&endpoint.maxbias0005b1000pt50f1000_asym_rebinned1.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_asym -o maxbias0005b1000pt50f1000_asym_rebinned5.root  -b0.0005 -f1000 -n1000 -r5  >&endpoint.maxbias0005b1000pt50f1000_asym_rebinned5.log&

./runEndpoint.py -i maxbias0001b1000pt50f1000_asym -o maxbias0001b1000pt50f1000_asym_rebinned25.root -b0.0001 -f1000 -n1000 -r25 >&endpoint.maxbias0001b1000pt50f1000_asym_rebinned25.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_asym -o maxbias0001b1000pt50f1000_asym_rebinned50.root -b0.0001 -f1000 -n1000 -r50 >&endpoint.maxbias0001b1000pt50f1000_asym_rebinned50.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_asym -o maxbias0001b1000pt50f1000_asym_rebinned10.root -b0.0001 -f1000 -n1000 -r10 >&endpoint.maxbias0001b1000pt50f1000_asym_rebinned10.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_asym -o maxbias0001b1000pt50f1000_asym_rebinned1.root  -b0.0001 -f1000 -n1000 -r1  >&endpoint.maxbias0001b1000pt50f1000_asym_rebinned1.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_asym -o maxbias0001b1000pt50f1000_asym_rebinned5.root  -b0.0001 -f1000 -n1000 -r5  >&endpoint.maxbias0001b1000pt50f1000_asym_rebinned5.log&

./runEndpoint.py -i maxbias005b1000pt50f1000_sym -o maxbias005b1000pt50f1000_sym_rebinned25.root -b0.005 -f1000 -n1000 -r25 -s >&endpoint.maxbias005b1000pt50f1000_sym_rebinned25.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_sym -o maxbias005b1000pt50f1000_sym_rebinned50.root -b0.005 -f1000 -n1000 -r50 -s >&endpoint.maxbias005b1000pt50f1000_sym_rebinned50.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_sym -o maxbias005b1000pt50f1000_sym_rebinned10.root -b0.005 -f1000 -n1000 -r10 -s >&endpoint.maxbias005b1000pt50f1000_sym_rebinned10.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_sym -o maxbias005b1000pt50f1000_sym_rebinned1.root  -b0.005 -f1000 -n1000 -r1  -s >&endpoint.maxbias005b1000pt50f1000_sym_rebinned1.log&
./runEndpoint.py -i maxbias005b1000pt50f1000_sym -o maxbias005b1000pt50f1000_sym_rebinned5.root  -b0.005 -f1000 -n1000 -r5  -s >&endpoint.maxbias005b1000pt50f1000_sym_rebinned5.log&

./runEndpoint.py -i maxbias001b1000pt50f1000_sym -o maxbias001b1000pt50f1000_sym_rebinned25.root -b0.001 -f1000 -n1000 -r25 -s >&endpoint.maxbias001b1000pt50f1000_sym_rebinned25.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_sym -o maxbias001b1000pt50f1000_sym_rebinned50.root -b0.001 -f1000 -n1000 -r50 -s >&endpoint.maxbias001b1000pt50f1000_sym_rebinned50.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_sym -o maxbias001b1000pt50f1000_sym_rebinned10.root -b0.001 -f1000 -n1000 -r10 -s >&endpoint.maxbias001b1000pt50f1000_sym_rebinned10.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_sym -o maxbias001b1000pt50f1000_sym_rebinned1.root  -b0.001 -f1000 -n1000 -r1  -s >&endpoint.maxbias001b1000pt50f1000_sym_rebinned1.log&
./runEndpoint.py -i maxbias001b1000pt50f1000_sym -o maxbias001b1000pt50f1000_sym_rebinned5.root  -b0.001 -f1000 -n1000 -r5  -s >&endpoint.maxbias001b1000pt50f1000_sym_rebinned5.log&

./runEndpoint.py -i maxbias0005b1000pt50f1000_sym -o maxbias0005b1000pt50f1000_sym_rebinned25.root -b0.0005 -f1000 -n1000 -r25 -s >&endpoint.maxbias0005b1000pt50f1000_sym_rebinned25.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_sym -o maxbias0005b1000pt50f1000_sym_rebinned50.root -b0.0005 -f1000 -n1000 -r50 -s >&endpoint.maxbias0005b1000pt50f1000_sym_rebinned50.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_sym -o maxbias0005b1000pt50f1000_sym_rebinned10.root -b0.0005 -f1000 -n1000 -r10 -s >&endpoint.maxbias0005b1000pt50f1000_sym_rebinned10.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_sym -o maxbias0005b1000pt50f1000_sym_rebinned1.root  -b0.0005 -f1000 -n1000 -r1  -s >&endpoint.maxbias0005b1000pt50f1000_sym_rebinned1.log&
./runEndpoint.py -i maxbias0005b1000pt50f1000_sym -o maxbias0005b1000pt50f1000_sym_rebinned5.root  -b0.0005 -f1000 -n1000 -r5  -s >&endpoint.maxbias0005b1000pt50f1000_sym_rebinned5.log&

./runEndpoint.py -i maxbias0001b1000pt50f1000_sym -o maxbias0001b1000pt50f1000_sym_rebinned25.root -b0.0001 -f1000 -n1000 -r25 -s >&endpoint.maxbias0001b1000pt50f1000_sym_rebinned25.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_sym -o maxbias0001b1000pt50f1000_sym_rebinned50.root -b0.0001 -f1000 -n1000 -r50 -s >&endpoint.maxbias0001b1000pt50f1000_sym_rebinned50.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_sym -o maxbias0001b1000pt50f1000_sym_rebinned10.root -b0.0001 -f1000 -n1000 -r10 -s >&endpoint.maxbias0001b1000pt50f1000_sym_rebinned10.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_sym -o maxbias0001b1000pt50f1000_sym_rebinned1.root  -b0.0001 -f1000 -n1000 -r1  -s >&endpoint.maxbias0001b1000pt50f1000_sym_rebinned1.log&
./runEndpoint.py -i maxbias0001b1000pt50f1000_sym -o maxbias0001b1000pt50f1000_sym_rebinned5.root  -b0.0001 -f1000 -n1000 -r5  -s >&endpoint.maxbias0001b1000pt50f1000_sym_rebinned5.log&
