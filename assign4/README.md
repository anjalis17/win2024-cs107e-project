Hellooo Liana and Kenny!
I've implemented the 2nd extension here, Mini-Valgrind. YAY!
I made it a little harder than it needed to be for myself because I chose to split apart the block metadata into 2 headers to avoid consuming any additional/unused space while meeting the 8-byte alignment criteria. In the end, it all worked out though (!!), and it was so satisfying to see it come together. 

A few notes about my implementation:

Here is a visual for how each of my blocks are structured--
+=================+===========+=============+===========+===========+======================+
|(1) Payload size |  (2) RZ1  | (3) Payload |  (4) RZ2  | (5) Status| (6) Backtrace Frames |
+=================+===========+=============+===========+===========+======================+

(1) and (2) make up part one of my block header (often referred to as hdr1), packaged into the 8-byte struct header_p1. 
(4), (5), and (6) make up part two of my block header (often referred to as hdr2), packaged into the 32-byte struct header_p2. 

My second redzone is implemented to start after the payload space (3). The size of the payload space is the size requested by the user rounded up to the nearest 8, to preserve 8-byte alignment. 
Thus, a memory error is thrown only when the user writes into either redzone. (If they write a couple bytes over what they initially requested for, and the location of those bytes falls in the payload space and NOT the redzone, then no error is thrown.) I thought this could be okay since that space is anyways unused/blocked off while the block is IN_USE. I spoke to Kenny about this during office hours and he said it was okay, but just wanted to make a note here as well. 

THANK YOU so much for reading and for your support!!