rm pre
gcc -DDEBUG -D_32_ -O3 -pthread -m96bit-long-double -fno-stack-protector -opre pre.c -lrt
