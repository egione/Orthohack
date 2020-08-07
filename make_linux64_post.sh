rm post
gcc -DDEBUG -D_64_ -O3 -pthread -m96bit-long-double -fno-stack-protector -opost post.c -lrt
