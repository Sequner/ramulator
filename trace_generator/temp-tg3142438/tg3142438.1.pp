#
# Format:
#   A region tag must preceed its start and end tags with no other
#   tags in between:
#     region <region-id> <weight> <n-start-points> <n-end-points> <length> <icount-from-program-start>
#     slice <slice-id> <phase-id> <distance-from-centroid>
#     start <marked-instr-id> <execution-count-since-program-start> <icount-from-slice-start>
#     end   <marked-instr-id> <execution-count-since-program-start> <icount-from-slice-start>
#
#   A markedInstrs tag must preceed the mark tags.  There must be
#   exactly one markedInstrs tag in the file.
#     markedInstrs <n-marked-instrs>
#     mark <mark-id> <instr-address> <used-in-start> <used-in-end> <is-nop-instr>
#
# I: 0


thread 0
version 3
# C: sum:dummy Command: /home/sanzhar/research/ramulator/trace_generator/pin-3.7-97619-g0d0c92f4f-gcc-linux/intel64/bin/pinbin -p32 /home/sanzhar/research/ramulator/trace_generator/pin-3.7-97619-g0d0c92f4f-gcc-linux/ia32/bin/pinbin -t src/PinPoints/obj-intel64/isimpoint.so -slice_size 10000 -o temp-tg3142438/tg3142438 -- ls -al

#Pinpoint= 1 Slice= 0  Icount= 0  Len= 10000
region 1 4.040 1 1 10000 0
slice 0 1 0.092763
warmup_factor 0 
start 1 1  0
end   2 476 0

#Pinpoint= 2 Slice= 10  Icount= 100000  Len= 10000
region 2 2.020 1 1 10000 100000
slice 10 7 0.404210
warmup_factor 0 
start 3 389  0
end   4 1158 0

#Pinpoint= 3 Slice= 22  Icount= 220000  Len= 10000
region 3 22.222 1 1 10000 220000
slice 22 3 0.054847
warmup_factor 0 
start 5 193  0
end   6 362 0

#Pinpoint= 4 Slice= 31  Icount= 310000  Len= 10000
region 4 7.071 1 1 10000 310000
slice 31 5 0.036737
warmup_factor 0 
start 7 454  0
end   8 356 0

#Pinpoint= 5 Slice= 39  Icount= 390000  Len= 10000
region 5 3.030 1 1 10000 390000
slice 39 6 0.072211
warmup_factor 0 
start 9 143  0
end   10 40 0

#Pinpoint= 6 Slice= 60  Icount= 600000  Len= 10000
region 6 8.081 1 1 10000 600000
slice 60 2 0.091889
warmup_factor 0 
start 11 756  0
end   12 302 0

#Pinpoint= 7 Slice= 69  Icount= 690000  Len= 10000
region 7 14.141 1 1 10000 690000
slice 69 0 0.133789
warmup_factor 0 
start 13 74  0
end   14 13 0

#Pinpoint= 8 Slice= 79  Icount= 790000  Len= 10000
region 8 13.131 1 1 10000 790000
slice 79 8 0.046768
warmup_factor 0 
start 15 147  0
end   16 210 0

#Pinpoint= 9 Slice= 95  Icount= 950000  Len= 10000
region 9 26.263 1 1 10000 950000
slice 95 4 0.081307
warmup_factor 0 
start 17 94  0
end   18 89 0

markedInstrs 18
mark-symbolic 1 0x7ffff7fd0100 1 0 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0x100
mark-symbolic 2 0x7ffff7fe9145 0 1 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0x19145
mark-symbolic 3 0x7ffff7fdd2fd 1 0 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0xd2fd
mark-symbolic 4 0x7ffff7fdd31a 0 1 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0xd31a
mark-symbolic 5 0x7ffff7fee678 1 0 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0x1e678
mark-symbolic 6 0x7ffff7fee646 0 1 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0x1e646
mark-symbolic 7 0x7fffe426c598 1 0 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x15f598
mark-symbolic 8 0x7fffe426c534 0 1 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x15f534
mark-symbolic 9 0x7fffe413eaa3 1 0 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x31aa3
mark-symbolic 10 0x7fffe412f584 0 1 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x22584
mark-symbolic 11 0x7ffff7fee613 1 0 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0x1e613
mark-symbolic 12 0x7ffff7fe1f9a 0 1 0 /lib64/ld-linux-x86-64.so.2 0x7ffff7fd0000 0x11f9a
mark-symbolic 13 0x7fffe41a5292 1 0 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x98292
mark-symbolic 14 0x7fffe25f9671 0 1 0 /lib/x86_64-linux-gnu/libnss_files.so.2 0x7fffe25f3000 0x6671
mark-symbolic 15 0x7fffe41a715a 1 0 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x9a15a
mark-symbolic 16 0x7fffe41a718d 0 1 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x9a18d
mark-symbolic 17 0x7fffe413f8f8 1 0 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0x328f8
mark-symbolic 18 0x7fffe41dcf60 0 1 0 /lib/x86_64-linux-gnu/libc.so.6 0x7fffe410d000 0xcff60

totalIcount 981003
pinpoints 9
endp
