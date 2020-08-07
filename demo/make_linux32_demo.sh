rm dct_8x8_demo
rm idct_8x8_demo
gcc -D_32_ -DNODEBUG -DTRANSFORM_FORWARD -O3 -m96bit-long-double -fno-stack-protector -odct_8x8_demo dct_idct_8x8_demo.c -lrt
gcc -D_32_ -DNODEBUG -DTRANSFORM_INVERSE -O3 -m96bit-long-double -fno-stack-protector -oidct_8x8_demo dct_idct_8x8_demo.c -lrt
