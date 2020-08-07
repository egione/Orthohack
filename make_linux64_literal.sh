rm literal
gcc -DDEBUG -D_64_ -O3 -m96bit-long-double -fno-stack-protector -oliteral literal.c -lrt
