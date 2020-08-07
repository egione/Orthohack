rm verify
gcc -DDEBUG -D_64_ -O3 -m96bit-long-double -fno-stack-protector -overify verify.c -lrt
