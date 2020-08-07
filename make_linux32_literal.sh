rm literal
gcc -DDEBUG -D_32_ -O3 -m96bit-long-double -fno-stack-protector -oliteral literal.c -lrt
