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
## Orthohack DCT & IDCT Demo
##
## MATLAB or GNU Octave code for testing the 8x8 real discrete cosine transform and its inverse.
##
function dct_8x8_demo()
  format long E;
  x=zeros(8,8);
  idx=0;
  for b=1:8;
    for a=1:8;
      if(mod(idx,3)==0)
        x(a,b)=idx/97;
      elseif(mod(idx,3)==1)
        x(a,b)=-idx/137;
      else
        x(a,b)=-idx/71;
      end
      idx++;
    end
  end
  printf('\nInput matrix X:\n\n');
  disp(x);
  printf('\nDCT2(X):\n\n');
  dct2_of_x=dct2(x,8,8);
  disp(dct2_of_x);
  printf('\nIDCT2(X):\n\n');
  idct2_of_x=idct2(x,8,8);
  disp(idct2_of_x);
endfunction
