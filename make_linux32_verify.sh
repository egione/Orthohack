rm verify
gcc -DDEBUG -D_32_ -O3 -m96bit-long-double -fno-stack-protector -overify verify.c -lrt
