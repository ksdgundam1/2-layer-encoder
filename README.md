# 2-layer-encoder-decoder
assignment from wireless communication

first layer of encoding is using CRC-8 encoder, which encode 16 bit binary data into 24 bit data
Divisor(a.k.a. generator) of this coding method is G = X ^ 8 + X ^ 2 + X + 1

second layer of encoding is using (7,4) hamming encoder, which converse 4 bit binary data (which is part of CRC output) into 7 bit data
matrix

Finally, (24 / 4) * 7 = 42 bit encoded data is printed
