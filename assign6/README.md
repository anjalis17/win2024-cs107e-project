Hi! It took me a SUPER LONG time to get my anti-aliased lines, triangles, and hardware floating point all working... But they were so satisfying to finally see!! Here's the story and my responses:

I first started with Bresenham's algorithm which I learned by reading this Wikipedia page: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm. I sketched and took notes by hand to understand the algorithm; I've uploaded photos of my notes detailing my understanding of the algorithm to the folder. This gave me ALIASED lines.

I then realized that we want to do ANTI-aliased lines, so I researched into Wu's algorithm. I learned the algorithm by studying and skimming the pseudocode found at (1) https://www.geeksforgeeks.org/anti-aliased-line-xiaolin-wus-algorithm/, and (2) https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm. I've described my understanding of how Wu's algorithm operates in the context of my implementation inline in my code via thorough line-by-line comments. My module now has functions to support both antialiased line drawing (default through gl_draw_line) AND aliased line drawing!!

Next, I drew my antialiased triangles!! YAY! I tried working my way through the resource linked in the assignment handout (triangle wireframes), but I didn't understand it fully and thought there had to be a simpler way. After sleeping on it, an idea hit me that I could look at the rectangular bounding box that circumscribes the triangle by considering the min and max x and y values of the vertices. This would give me an iterable range of pixels to check to see if they were inside the triangle (and if so, I could fill them with the specified color)! With my range of pixels to check, the question became how to determine if a pixel lies inside my triangle. I found this resource (https://www.geeksforgeeks.org/check-whether-a-given-point-lies-inside-a-triangle-or-not/), which helped me realize that a point P lies inside triangle ABC if the area of triangle ABC can be fragmented into three subareas which meet at P. In other words, PAC + PAB + PBC = ABC if and only if P is in ABC! My implementation uses this approach and I used the mathematical formula from the same webpage to calculate area of a triangle given three vertices.

I also spent a very long time reading the RISC V manual and finally found out that I would need to use the CSRW instruction to write to enable the register that controls floating point operations (FS) in mstatus. I knew assembly was needed to write a global function like we did in assign2 with timer.asm, but I was puzzled as to how to implement the exact instructions and what changes to make to my Makefile. Lisa directed me to Julie's interrupts assembly code which I used as reference when writing my own assembly. I figured out that I needed to add an f at the end of the -march value in my Makefile, but even after this, I was still getting a blinking light from the Pi. I explained what I did to Lisa during lab and she told me that she had to have a header file for her assembly. I didn't end up making a header file, but simply putting the assembly function prototype into my gl.c file did the trick. YAY! 
In summary, the three changes needed to configure hardware floating point were:
1) Add f to -march in Makefile under ARCH flags (so we get -march = rv64imf)
2) Write the assembly file (config_fp.s) to enable hardware floating point in the mstatus register
3) Put the assembly function prototype into the .c file it's being used in (gl.c) and call the function in gl_init()

My time comparisons revealed that hard-float is indeed faster and more efficient that soft float! I did these tests by running my test_triangle() function (drawing anti-aliased triangles, which also calls draw anti-alised line function) in test_gl_console.c:

HARD FLOAT TIMES
13756555 microseconds = 13.757 seconds - with double buffer mode
25905283 microseconds = 25.905 seconds - with single buffer mode

SOFT FLOAT TIMES
17797916 microseconds = 17.800 seconds - with double buffer mode
30328796 = 30.329 seconds - with single buffer mode 

THANK YOU! ~Anjali