/*
Orthohack
Copyright 2016 Russell Leidich
http://orthohack.blogspot.com

This collection of files constitutes the Orthohack Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Orthohack Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Orthohack Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Orthohack Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
FILE *
file_open(u8 *name_base,u8 mode){
  u8 *mode_string_base;

  switch(mode){
  case FILE_READ:
    mode_string_base="rb";
    break;
  case FILE_WRITE:
    mode_string_base="wb";
    break;
  case FILE_APPEND:
    mode_string_base="ab";
  }
  return fopen((const char *)(name_base),(const char *)(mode_string_base));
}

u8
file_read(u64 string_size,u8 *string_base,FILE *handle){
  u8 status;
  u64 transfer_size;

#ifdef _32_
  if((string_size>>32)==0){
#endif
    transfer_size=(u64)(fread(string_base,(size_t)(U8_SIZE),(size_t)(string_size),handle));
#ifdef _32_
  }else{
    transfer_size=0;
  }
#endif
  return (string_size!=transfer_size);
}

u8
file_write(u64 string_size,u8 *string_base,FILE *handle){
  u8 status;
  u64 transfer_size;

#ifdef _32_
  if((string_size>>32)==0){
#endif
    transfer_size=(u64)(fwrite(string_base,(size_t)(U8_SIZE),(size_t)(string_size),handle));
#ifdef _32_
  }else{
    transfer_size=0;
  }
#endif
  return (string_size!=transfer_size);
}

u8
file_close(FILE **handle_base){
/*
Out:

  *handle_base=NULL because it's no longer valid, regardless of whether fclose() succeeded.
*/
  u8 status;

  status=0;
  if(*handle_base){
    status=(fclose(*handle_base)!=0);
    *handle_base=NULL;
  }
  return status;
}

u8
file_rewind(FILE *handle){
/*
Seek to the base of a file. (We use fseeko() instead of rewind() because the latter was never formally defined for 64-bit stream offsets.)

In:

  handle is the handle of the file to manipulate.

Out:

  Returns 1 on failure, else 0.
*/
  return !!fseeko(handle,(off_t)(0),SEEK_SET);
}

u8
file_size_get_rewind(FILE *handle,u64 *size_base){
/*
Out:
  Returns 0 if *file_size_base is updated and the file is seeked to 0, else 1.
*/
  i64 offset;
  u8 status;

  status=1;
  if(fseeko(handle,(off_t)(0),SEEK_END)==0){
    if((offset=(i64)(ftello(handle)))>=0){
      if((status=file_rewind(handle))==0){
        *size_base=(u64)(offset);
      }
    }
  }
  return status;
}
