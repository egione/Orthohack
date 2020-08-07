##
## Orthohack
## Copyright 2016 Russell Leidich
## http://orthohack.blogspot.com
##
## This collection of files constitutes the Orthohack Library. (This is a
## library in the abstact sense; it's not intended to compile to a ".lib"
## file.)
##
## The Orthohack Library is free software: you can redistribute it and/or
## modify it under the terms of the GNU Limited General Public License as
## published by the Free Software Foundation, version 3.
##
## The Orthohack Library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## Limited General Public License version 3 for more details.
##
## You should have received a copy of the GNU Limited General Public
## License version 3 along with the Orthohack Library (filename
## "COPYING"). If not, see http://www.gnu.org/licenses/ .
##
## Orthohack Orthobasis Database Generator Demo
##
## Really crap MATLAB or GNU Octave code for capturing the NxN (2D) forward and inverse real discrete cosine transform orthobasis matrices as (N^2)-element 1D vectors. Run it once, and be done with it!
##
## In:
##
##   dct_pathname is the target folder for the (forward) DCT orthobasis database, which MUST end with a path separator ("/" or "\", depending in your OS).
##
##   idct_pathname is the same as dct_pathname, but for the inverse DCT.
##
##   n is the number of samples per side, i.e. the full DCT matrix converts N^2 samples to N^2 coefficients, N<=31,622 (but don't try more than 100x100 unless you have monstrous memory bus bandwidth on which to run the preprocessor).
##
## Out:
##
##   N^2 files 000000000.bin, 00000001.bin, etc. appear at both dct_pathname and idct_pathname, each containing N^2 double-precision floats corresponding to all the spatial samples in a particular orthobasis vector of the forward or inverse 2D real DCT, respectively.
##
function dct2_database(dct_pathname,idct_pathname,n)
  mkdir(dct_pathname);
  mkdir(idct_pathname);
  x=zeros(n,n);
  file_idx=0;
  for b=1:n;
    for a=1:n;
##
## Use the inverse tranform of (each coefficient as 1 with others 0) to generate the orthobases for the forward transform.
##
      x(a,b)=1;
      y=idct2(x,n,n);
      filename=strcat(dct_pathname,num2str(file_idx,'%09d.bin'));
      printf('Writing DCT basis vector %s ...\n',filename);
      handle=fopen(filename,'w');
      fwrite(handle,y,'double');
      fclose(handle);
      y=dct2(x,n,n);
      x(a,b)=0;
      filename=strcat(idct_pathname,num2str(file_idx,'%09d.bin'));
      printf('Writing IDCT basis vector %s ...\n',filename);
      handle=fopen(filename,'w');
      fwrite(handle,y,'double');
      fclose(handle);
      file_idx++;
    end
  end
  filesize=n*n*8;
  printf('Done. Go check that all files have %d bytes.\n',filesize);
endfunction
