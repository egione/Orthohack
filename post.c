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
#include "unix_include.h"
#include "build.h"
#include "orthoconstant.h"
#include "config.h"
#include "debug.h"
#include "spawn.h"
#include "file_sys.h"
#include "sort.h"
#include "post.h"
#include "orthodim.h"
#include "orthonumber.h"
#include "number_io.h"
#include "orthofixed.h"
#include "orthocustom.h"
#include "tree.h"
#include "ohk_header.h"
#include "print.c"
#include "debug.c"
#include "string.c"
#include "spawn.c"
#include "sort.c"
#include "file_sys.c"
#include "number.c"
#include "pack.c"

void
print_error_exit_input_file_corrupt(void){
  print_error_exit("Input file is corrupt, or maybe its numerical precision doesn't match PRECISION_IN in post.h");
  return;
}

void
print_none_found(void){
  print(" none found.\n");
  return;
}

void
print_warning_term(u8 *string_base){
  printf("\nWARNING: %s found. The preprocessor should have removed them.\nContinuing...\n",string_base);
  fflush(stdout);
  return;
}

u8
bit_count_get(u64 value){
  u8 bit_count;

  MSB_GET(value,bit_count);
  bit_count++;
  return bit_count;
}

u64
transform_make(u8 *alias_count_bit_count_base,u64 alias_idx_max,alias_t *alias_list_base,u8 *monomial_count_bit_count_base,u64 *transform_string_base){
/*
Convert a list of (alias_t)s to a transform instruction string.

In:

  *alias_count_bit_count_base is undefined.

  alias_idx_max is the number of (alias_t)s at alias_list_base, minus 1.

  *alias_list_base contains (alias_idx_max+1) (alias_t)s.

  *monomial_count_bit_count_base is undefined.

  *transform_string_base is writable for size ((((monomial_idx_max+1)<<1)+((alias_idx_max+1)<<1))<<U64_SIZE_LOG2), where monomial_idx_max is the sample count in the original orthobasis, plus the number of binomials, minus 1. This is enough to accomodate the transform size. The transform looks like this:

    1. 1 u64: (Non)zero monomial count (initially, zero count) of how many monomials in a row are (not) zero. Definition toggles each time.

    2. 1 u64: Alias count. This is the number of (ratio_idx, coeff_idx) pairs that follow, >0.

    3. ((#2)<<1) (u64)s: Vectors of: ratio_idx followed by coeff_idx for this monomial.

Out:

  Returns the maximum index of *transform_string_base.

  *alias_count_bit_count_base contains the number of bits required to store the value described in In:transform_string_base:(#1).

  *monomial_count_bit_count_base contains the number of bits required to store the value described in In:transform_string_base:(#1).

  *transform_string_base is serially encoded as defined in In:transform_string_base.
*/
  u64 alias_count;
  u64 alias_count_max;
  u64 alias_idx;
  u64 alias_idx_prv;
  u8 bit_count;
  u64 monomial_idx;
  u64 monomial_idx_prv;
  u64 monomial_count;
  u64 monomial_count_max;
  u64 transform_alias_count_idx;
  u64 transform_idx;
  u64 transform_monomial_count_idx;

  monomial_idx_prv=alias_list_base->monomial_idx;
  monomial_count=monomial_idx_prv;
  monomial_count_max=monomial_idx_prv;
/*
Store the number of 0 monomials until the first nonzero one.
*/
  *transform_string_base=monomial_idx_prv;
  transform_monomial_count_idx=1;
  transform_string_base[transform_monomial_count_idx]=1;
  transform_alias_count_idx=2;
  transform_idx=3;
  alias_idx=0;
  alias_idx_prv=0;
  alias_count_max=0;
  do{
    monomial_idx=alias_list_base[alias_idx].monomial_idx;
    if(monomial_idx!=monomial_idx_prv){
      alias_count=alias_idx-alias_idx_prv;
      if(alias_count>alias_count_max){
        alias_count_max=alias_count;
      }
      alias_idx_prv=alias_idx;
      transform_string_base[transform_alias_count_idx]=alias_count;
      monomial_count=monomial_idx-monomial_idx_prv-1;
      monomial_idx_prv=monomial_idx;
      if(!monomial_count){
        transform_string_base[transform_monomial_count_idx]++;
      }else{
        transform_string_base[transform_idx]=monomial_count;
        transform_idx++;
        if(monomial_count>monomial_count_max){
          monomial_count_max=monomial_count;
        }
        monomial_count=transform_string_base[transform_monomial_count_idx];
        if(monomial_count>monomial_count_max){
          monomial_count_max=monomial_count;
        }
        transform_string_base[transform_idx]=1;
        transform_monomial_count_idx=transform_idx;
        transform_idx++;
      }
      transform_alias_count_idx=transform_idx;
      transform_idx++;
    }
    transform_string_base[transform_idx]=alias_list_base[alias_idx].ratio_idx;
    transform_string_base[transform_idx+1]=alias_list_base[alias_idx].coeff_idx;
    transform_idx+=2;
  }while((alias_idx++)!=alias_idx_max);
  monomial_count=transform_string_base[transform_monomial_count_idx];
  if(monomial_count>monomial_count_max){
    monomial_count_max=monomial_count;
  }
  bit_count=bit_count_get(monomial_count_max);
  *monomial_count_bit_count_base=bit_count;
  alias_count=alias_idx-alias_idx_prv;
  if(alias_count>alias_count_max){
    alias_count_max=alias_count;
  }
  transform_string_base[transform_alias_count_idx]=alias_count;
  bit_count=bit_count_get(alias_count_max);
  *alias_count_bit_count_base=bit_count;
  transform_idx--;
  return transform_idx;
}

u64
mask_get(u8 bit_count){
  u64 mask;

  mask=0;
  if(bit_count){
    bit_count--;
    mask++;
    mask<<=1;
    mask<<=bit_count;
    mask--;
  }
  return mask;
}

int
main(int argc, char *argv[]){
  u64 alias_count;
  u8 alias_count_bit_count;
  u64 *alias_idx_list_base;
  alias_t *alias_list_base;
  alias_t alias_scratch;
  u64 binomial_count;
  u64 binomial_idx_read;
  u64 binomial_idx_write;
  binomial_remap_t *binomial_list_base;
  u64 binomial_list_u64_count;
  u64 coeff_count;
  u64 coeff_idx;
  u8 coeff_idx_bit_count;
  u8 column_idx;
  CUSTOM custom;
  CUSTOM *custom_list_base;
  u8 custom_list_invalid_status;
  NUMBER denoising_error;
  NUMBER denoising_error_max;
  u64 denominator_idx;
  u8 *file_in_base;
  u64 file_in_idx;
  u64 file_in_size;
  u64 file_out_size;
  FIXED fixed;
  FIXED *fixed_list_base;
  u8 fixed_list_invalid_status;
  FILE *handle_in;
  orthohack_header_t header;
  NUMBER ignored_number;
  u64 ignored_u64;
  u64 mask;
  monomial_remap_t **monomial_base_list_base;
  u64 monomial_count;
  u8 monomial_count_bit_count;
  u64 *monomial_count_list_base;
  u64 monomial_count_sum;
  u8 monomial_idx_bit_count;
  u64 monomial_idx_max;
  u64 monomial_idx_read;
  u64 monomial_idx_write;
  monomial_remap_t *monomial_list_base;
  NUMBER number0;
  NUMBER number1;
  u64 number_count;
  u8 number_idx_bit_count;
  u64 number_idx_max;
  u64 number_idx_read;
  u64 number_idx_write;
  NUMBER_IN number_in;
  NUMBER *number_list_base;
  u8 *number_list_bitmap_base;
  NUMBER *number_list_consolidated_base;
  u64 number_list_idx_max;
  NUMBER *number_list_optimized_base;
  u8 *number_list_sign_bitmap_base;
  u64 number_list_size;
  NUMBER_OUT number_out;
  u64 numerator_idx;
  u8 packed_bit_count;
  u64 packed_hi;
  u64 packed_lo;
  u8 progress_status;
  u8 progress_status_prv;
  u64 ratio_idx;
  u64 sample_idx;
  u64 sample_idx_max;
  u8 sign;
  u64 *sort_string_base;
  sort_tag_t *sort_tag_list0_base;
  sort_tag_t *sort_tag_list1_base;
  u8 status;
  u64 transform_idx;
  u64 transform_idx_max;
  u64 transform_size;
  u64 *transform_string_base;
  u64 transform_string_u64_count;
  u64 value;

  status=0;
  fixed_list_invalid_status=1;
  custom_list_invalid_status=1;
  printf("/*\n");
  print_intro("Postprocessor");
#if (PRECISION_IN==2)||(PRECISION_INTERNAL==2)||(PRECISION_FLOATING==2)
  if(sizeof(long double)!=sizeof(u96_t)){
    print_error_exit("80-bit precision is not available. Change PRECISION_IN, PRECISION_INTERNAL, and PRECISION_FLOATING accordingly.");
  }
#endif
  print_precision(PRECISION_IN,0);
  print_precision(PRECISION_INTERNAL,1);
  printf("Reading %s...\n",filename_base);
  fflush(stdout);
  handle_in=file_open(filename_base,FILE_READ);
  if(!handle_in){
    print_error_exit("Cannot read input file");
  }
  file_size_get_rewind(handle_in,&file_in_size);
  file_in_base=NULL;
  if(file_in_size>sizeof(orthohack_header_t)){
    file_in_base=string_create(file_in_size);
    status=file_read(file_in_size,file_in_base,handle_in);
  }
  status|=file_close(&handle_in);
  memcpy((void *)(&header),(const void *)(file_in_base),sizeof(orthohack_header_t));
  if(status||(header.number_component_idx_max!=NUMBER_COMPONENT_IDX_MAX)||(((header.number_u16_idx_max+1)<<1)!=sizeof(number_in_unpadded_t))){
    print_error_exit_input_file_corrupt();
  }
  DEBUG_DOUBLE("precision_ratio_limit_input",header.precision_ratio_limit_minus_1+1.0f);
  DEBUG_DOUBLE("precision_ratio_limit_local",NUMBER_MAKE(PRECISION_RATIO_LIMIT));
  DEBUG_DOUBLE("precision_zero_limit_input",header.precision_zero_limit);
  coeff_count=header.coeff_idx_max+1;
  monomial_base_list_base=(monomial_remap_t **)(string_item_list_create_zero(coeff_count,sizeof(monomial_remap_t **)));
  status=!monomial_base_list_base;
  monomial_count_list_base=(u64 *)(string_item_list_create_zero(coeff_count,U64_SIZE));
  status|=!monomial_count_list_base;
/*
Allocate space for sample_idx==0, which will always contain 0.
*/
  header.sample_idx_max++;
  DEBUG_U64("sample_count_after_inserting_zero",header.sample_idx_max+1);
  DEBUG_U64("coeff_count",header.coeff_idx_max+1);
/*
number_idx_max will be the maximum index of *number_list_base, which is just the sum of the monomial count (accrued below) and the binomial count, minus 1.
*/
  number_idx_max=header.binomial_idx_max;
  binomial_count=header.binomial_idx_max+1;
  DEBUG_U64("binomial_count",binomial_count);
/*
monomial_idx_max is the maximum orthobasis sample index plus the number of binomials.
*/
  monomial_idx_max=header.sample_idx_max+binomial_count;
  if(status||(header.signature!=ORTHOHACK_FILE_SIGNATURE)||(file_in_size<((U64_SIZE<<1)+header.size_following))||(!binomial_count)||(monomial_idx_max<binomial_count)){
    print_error_exit_input_file_corrupt();
  }
  DEBUG_U64("monomial_count",monomial_idx_max+1);
  coeff_idx=0;
  file_in_idx=sizeof(orthohack_header_t);
  file_in_size=((U64_SIZE<<1)+header.size_following)-sizeof(orthohack_header_t);
  monomial_count_sum=0;
  do{
    if(file_in_size>=U64_SIZE){
      file_in_size-=U64_SIZE;
      memcpy((void *)(&monomial_count),(const void *)(&file_in_base[file_in_idx]),U64_SIZE);
      file_in_idx+=U64_SIZE;
      if(monomial_count){
        monomial_list_base=(monomial_remap_t *)(string_item_list_create(monomial_count,sizeof(monomial_remap_t)));
        status=!monomial_list_base;
        monomial_count_sum+=monomial_count;
      }else{
        monomial_list_base=NULL;
      }
      monomial_count_list_base[coeff_idx]=monomial_count;
      monomial_base_list_base[coeff_idx]=monomial_list_base;
      monomial_idx_write=0;
      while((!status)&&monomial_count--){
        if(file_in_size>=sizeof(monomial_in_t)){
          file_in_size-=sizeof(monomial_in_t);
          memcpy((void *)(&sample_idx),(const void *)(&file_in_base[file_in_idx]),U64_SIZE);
          file_in_idx+=U64_SIZE;
/*
Skip over sample_idx==0.
*/
          sample_idx++;
          if(sample_idx>monomial_idx_max){
            status=1;
          }
          monomial_list_base[monomial_idx_write].sample_idx=sample_idx;
          NUMBER_IN_PAD(&file_in_base[file_in_idx],number_in);
          file_in_idx+=sizeof(number_in_unpadded_t);
          monomial_list_base[monomial_idx_write].ratio=NUMBER_MAKE(number_in);          
        }else{
          status=1;
        }
        monomial_idx_write++;
      }
    }else{
      status=1;
    }
  }while((!status)&&((coeff_idx++)!=(header.coeff_idx_max)));
  alias_list_base=(alias_t *)(string_item_list_create(monomial_count_sum,sizeof(alias_t)));
  status|=!alias_list_base;
/*
*transform_string_base will contain the transform, which is a list of (u64)s who size is at most (((monomial_idx_max+1)+(monomial_count_sum<<1))<<U64_SIZE_LOG2) -- see transform_make().
*/
  transform_idx_max=((monomial_idx_max+1)<<1)+(monomial_count_sum<<1)-1;
  transform_string_base=(u64 *)(string_item_list_create(transform_idx_max+1,U64_SIZE));
  status|=!transform_string_base;
  alias_idx_list_base=(u64 *)(string_item_list_create(monomial_count_sum<<1,U64_SIZE));
  status|=!alias_idx_list_base;
  if(!status){
    number_idx_max+=monomial_count_sum;
    number_count=number_idx_max+1;
    DEBUG_U64("number_count_raw",number_count);
    binomial_list_base=(binomial_remap_t *)(string_item_list_create(binomial_count,sizeof(binomial_remap_t)));
    status=!binomial_list_base;
    binomial_idx_write=0;
    while((!status)&&(binomial_idx_write<=header.binomial_idx_max)){
      if(file_in_size>=sizeof(binomial_in_t)){
        file_in_size-=sizeof(binomial_in_t);
        memcpy((void *)(&denominator_idx),(const void *)(&file_in_base[file_in_idx]),U64_SIZE);
        file_in_idx+=U64_SIZE;
        memcpy((void *)(&numerator_idx),(const void *)(&file_in_base[file_in_idx]),U64_SIZE);
        file_in_idx+=U64_SIZE;
/*
Skip over sample_idx==0.
*/
        denominator_idx++;
        numerator_idx++;
        if((denominator_idx>monomial_idx_max)||(numerator_idx>monomial_idx_max)||(denominator_idx<=numerator_idx)){
          status=1;
        }
        binomial_list_base[binomial_idx_write].denominator_idx=denominator_idx;
        binomial_list_base[binomial_idx_write].numerator_idx=numerator_idx;
        NUMBER_IN_PAD(&file_in_base[file_in_idx],number_in);
        file_in_idx+=sizeof(number_in_unpadded_t);
        binomial_list_base[binomial_idx_write].ratio=NUMBER_MAKE(number_in);
      }else{
        status=1;
      }
      binomial_idx_write++;
    }
  }else{
    status=1;
  }
  if(status||file_in_size){
    print_error_exit_input_file_corrupt();
  }
  string_free(file_in_base);  
  print("Globally denoising all numbers in order to reduce the number of uniques...\n");
/*
Allocate the consolidated number list, which is the denoised uniques of all the numbers we encounter, forced to include 0.0 and 1.0 (hence number_idx_max+3 instead of +1).
*/
  number_list_consolidated_base=(NUMBER *)(string_item_list_create(number_idx_max+3,sizeof(NUMBER)));
  status=!number_list_consolidated_base;
  status|=string_u64_product(coeff_count,monomial_idx_max+1,&alias_count);
  if(status){
    alias_count=0;
  }
  sort_string_base=sort_init();
  status|=!sort_string_base;
/*
Allocate sort tag lists in accordance with the maximum number of items at number_list_consolidated_base.
*/
  sort_tag_list0_base=(sort_tag_t *)(string_item_list_create(number_idx_max+3,sizeof(sort_tag_t)));
  status|=!sort_tag_list0_base;
  sort_tag_list1_base=(sort_tag_t *)(string_item_list_create(number_idx_max+3,sizeof(sort_tag_t)));
  status|=!sort_tag_list1_base;
  number_list_base=(NUMBER *)(string_item_list_create(number_idx_max+3,sizeof(NUMBER)));
  status|=!number_list_base;
  number_list_sign_bitmap_base=string_bitmap_create(number_idx_max+3,1,0,&ignored_u64);
  status|=!number_list_sign_bitmap_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  coeff_idx=0;
/*
Load *number_list_base with all the (NUMBER)s that appear in the file. We need to consolidate them into a shorter list, which can then be made into a lookup table for cache efficiency.
*/
  number_idx_write=0;
  do{
    monomial_count=monomial_count_list_base[coeff_idx];
    monomial_list_base=monomial_base_list_base[coeff_idx];
    monomial_idx_read=0;
    while(monomial_count--){
      number_list_base[number_idx_write]=monomial_list_base[monomial_idx_read].ratio;      
      number_idx_write++;
      monomial_idx_read++;
    }
  }while((coeff_idx++)!=(header.coeff_idx_max));
  binomial_idx_read=0;
  do{
    number_list_base[number_idx_write]=binomial_list_base[binomial_idx_read].ratio;
    number_idx_write++;
  }while((binomial_idx_read++)!=header.binomial_idx_max);
  number_list_sign_zero(number_idx_max,number_list_base,number_list_sign_bitmap_base);
  string_free(number_list_sign_bitmap_base);
  number_list_consolidate(sort_string_base,sort_tag_list0_base,sort_tag_list1_base,number_list_base,number_list_consolidated_base,&number_idx_max,&ignored_number);
/*
Append 0.0 and 1.0 to *number_list_consolidated_base, which will can with optimization by deleting dead terms and degenerating some multiplies to adds or subtracts. We reserved space above.
*/
  number_idx_read=number_idx_max;
  number_idx_max+=2;
  number_idx_write=number_idx_max;
  SET_ONE(number0);
  do{
    number1=number_list_consolidated_base[number_idx_read];    
    if(IS_LESS(number1,number0)){
/*
Store 1.0 in sorted order. It might be present already, but number_list_consolidated_flush_zero_one() will remove any duplicates.
*/
      number_list_consolidated_base[number_idx_write]=number0;
      number_idx_write--;
      SET_ZERO(number0);
    }
    number_list_consolidated_base[number_idx_write]=number1;
    number_idx_write--;
  }while(number_idx_read--);
  if(IS_NONZERO(number0)){
/*
We never managed to store 1.0 above. Store it now.
*/
    number_list_consolidated_base[1]=number0;
  }
/*
Store 0.0. It might be present already, but number_list_consolidated_flush_zero_one() will remove any duplicates.
*/
  *number_list_consolidated_base=number0;  
  number_idx_max=number_list_consolidated_flush_zero_one(number_idx_max,number_list_consolidated_base);
/*
We just sorted a bunch of nonnegative numbers while coalescing neighbors of 0.0 and 1.0, to 0.0 and 1.0,respectively. So, conveniently, 0.0 is the first item on the list.
*/
  number_count=number_idx_max+1;
  DEBUG_U64("number_count_consolidated",number_count);
  DEBUG_NUMBER("magnitude_least_nonzero",number_list_consolidated_base[1]);
  DEBUG_NUMBER("magnitude_greatest",number_list_consolidated_base[number_idx_max]);
  number_list_optimized_base=(NUMBER *)(string_item_list_create(number_count,sizeof(NUMBER)));
  status=!number_list_optimized_base;  
  fixed_list_base=(FIXED *)(string_create(number_count*sizeof(FIXED)));
  status|=!fixed_list_base;
  custom_list_base=(CUSTOM *)(string_create(number_count*sizeof(CUSTOM)));
  status|=!custom_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
/*
Initialize the sort tag list for number_list_consolidated_base, so we can sort it decending by population for cache densification later. Start with item==U64_MAX, meaning no population. Population will then decrement upon discovery, so that after sorting ascending, the highest populations will appear concentrated at the least memory addresses. (What's important is really just that they're concentrated somewhere.)
*/
  number_idx_write=0;
  do{
    sort_tag_list0_base[number_idx_write].item=U64_MAX;
    sort_tag_list0_base[number_idx_write].struct_idx=number_idx_write;
  }while((number_idx_write++)<=number_idx_max);
  print("Denoising monomial and binomial ratios...\n");
  denoising_error_max=0.0f;
  coeff_idx=0;
  number_idx_read=0;
  monomial_idx_write=0;
  do{
    monomial_count=monomial_count_list_base[coeff_idx];
    monomial_list_base=monomial_base_list_base[coeff_idx];
    monomial_idx_read=0;
    while(monomial_count--){
      number0=monomial_list_base[monomial_idx_read].ratio;
      sign=SIGN_GET(number0);
      number0=SIGN_ZERO(number0);
      number_idx_read=number_list_find_nearest_idx(number0,number_idx_max,number_list_consolidated_base);
/*
Copy all (monomial_remap_t)s to (alias_t)s, except those which have been flushed to 0.0, i.e. which have number_idx_read==0.
*/
      if(number_idx_read){
        sample_idx=monomial_list_base[monomial_idx_read].sample_idx;
/*
Decrement the population for this number, which will allow us to sort ascending by population later.
*/
        sort_tag_list0_base[number_idx_read].item--;
        ratio_idx=(number_idx_read<<(NUMBER_COMPONENT_IDX_MAX+1))+sign;
        alias_list_base[monomial_idx_write].ratio_idx=ratio_idx;
        alias_list_base[monomial_idx_write].monomial_idx=sample_idx;
        alias_list_base[monomial_idx_write].coeff_idx=coeff_idx;
        monomial_idx_write++;
      }else{
        print_error_exit("Report this bug: monomial consolidation failed.\n");
      }
      number1=number_list_consolidated_base[number_idx_read];
      denoising_error=MAGNITUDE(DIFFERENCE(number0,number1));
      if(denoising_error>denoising_error_max){
        denoising_error_max=denoising_error;
      }
      monomial_idx_read++;
    }
  }while((coeff_idx++)!=(header.coeff_idx_max));
  binomial_idx_write=0;
  do{
    number0=binomial_list_base[binomial_idx_write].ratio;
    sign=SIGN_GET(number0);
    number0=SIGN_ZERO(number0);
    number_idx_read=number_list_find_nearest_idx(number0,number_idx_max,number_list_consolidated_base);
    if(!number_idx_read){
/*
number0 is +/-0.0. Force sign==0 so we don't get into issues with multidimensional negative 0s.
*/
      sign=0;
/*
At the moment, this is a fatal error. But later it might be acceptable under some circumstances, hence the sign clearing above.
*/
      print_error_exit("Report this bug: binomial consolidation failed.\n");
    }
/*
Decrement the population for this number, which will allow us to sort ascending by population later.
*/
    sort_tag_list0_base[number_idx_read].item--;
    ratio_idx=(number_idx_read<<(NUMBER_COMPONENT_IDX_MAX+1))+sign;
    binomial_list_base[binomial_idx_write].ratio_idx=ratio_idx;
    number1=number_list_consolidated_base[number_idx_read];
    denoising_error=MAGNITUDE(DIFFERENCE(number0,number1));
    if(denoising_error>denoising_error_max){
      denoising_error_max=denoising_error;
    }
  }while((binomial_idx_write++)!=header.binomial_idx_max);
  DEBUG_NUMBER("denoising_error_max",denoising_error_max);
  print("Making transform...\n");
/*
Sort *number_list_consolidated_base by population to number_list_optimized_base.
*/
  sort_tag_list_sort(sort_string_base,sizeof(NUMBER),(u8 *)(number_list_consolidated_base),(u8 *)(number_list_optimized_base),number_idx_max,&sort_tag_list0_base,&sort_tag_list1_base);
/*
Calculate new number indexes using *sort_tag_list1_base as a reindexing map, since it's big enough and otherwise undefined.
*/
  number_idx_read=0;
  do{
    number_idx_write=sort_tag_list0_base[number_idx_read].struct_idx;
    sort_tag_list1_base[number_idx_write].struct_idx=number_idx_read;
  }while((number_idx_read++)!=number_idx_max);
/*
Reassign all references to the number list at alias_list_base and binomial_list_base, so they match the cache-optimized offsets.
*/
  do{
    binomial_idx_write--;
    ratio_idx=binomial_list_base[binomial_idx_write].ratio_idx;
    sign=ratio_idx&1;
    ratio_idx>>=1;
    ratio_idx=sort_tag_list1_base[ratio_idx].struct_idx;
    ratio_idx<<=1;
    ratio_idx|=sign;
    binomial_list_base[binomial_idx_write].ratio_idx=ratio_idx;
  }while(binomial_idx_write);  
  do{
    monomial_idx_write--;
    ratio_idx=alias_list_base[monomial_idx_write].ratio_idx;
    sign=ratio_idx&1;
    ratio_idx>>=1;
    ratio_idx=sort_tag_list1_base[ratio_idx].struct_idx;
    ratio_idx<<=1;
    ratio_idx|=sign;
    alias_list_base[monomial_idx_write].ratio_idx=ratio_idx;
  }while(monomial_idx_write);
  fixed_list_invalid_status=0;
  custom_list_invalid_status=0;
  number_idx_read=0;
  do{
    number0=number_list_optimized_base[number_idx_read];
    if(IS_GREATER_EQUAL(number0,NUMBER_MAKE(U32_SPAN_HALF))||IS_LESS(number0,NUMBER_MAKE(I32_MIN))){
      fixed_list_invalid_status=1;
    }
    fixed_list_base[number_idx_read]=FIXED_FROM_NUMBER(number0);
    if(IS_GREATER_EQUAL(number0,NUMBER_MAKE(U32_SPAN_HALF>>CUSTOM_FRACTION_BIT_COUNT))||IS_LESS(number0,NUMBER_MAKE(I32_MIN>>CUSTOM_FRACTION_BIT_COUNT))){
      custom_list_invalid_status=1;
    }
    CUSTOM_FROM_NUMBER(number0,custom);
    custom_list_base[number_idx_read]=custom;
  }while((number_idx_read++)<=number_idx_max);
  sort_struct_list_sort(alias_idx_list_base,(u8 *)(&alias_scratch),sort_string_base,monomial_count_sum-1,(u8 *)(alias_list_base),sizeof(alias_t)-1);
  transform_idx_max=transform_make(&alias_count_bit_count,monomial_count_sum-1,alias_list_base,&monomial_count_bit_count,transform_string_base);
  DEBUG_U64("transform_idx_max",transform_idx_max);
  printf("*/\n");
  printf("#define LITERAL_LIST \\\n");
  number_idx_read=0;
  do{
    number0=number_list_optimized_base[number_idx_read];
    if(number_idx_read!=number_idx_max){
      printf("%1.16E, \\\n",(double)(number0));
    }else{
      printf("%1.16E\n",(double)(number0));
    }
  }while((number_idx_read++)!=number_idx_max);
  printf("\n");
  printf("#define NUMBER_LIST \\\n");
  string_zero(sizeof(NUMBER_OUT),(u8 *)(&number_out));
  column_idx=0;
  number_idx_read=0;
  do{
    number0=number_list_optimized_base[number_idx_read];
    number_out=NUMBER_OUT_MAKE(number0);
    column_idx=print_string_as_u8(column_idx,(u8 *)(&number_out),sizeof(NUMBER_OUT));
    column_idx=print_u8_string_end(column_idx,number_idx_read,number_idx_max);
  }while((number_idx_read++)!=number_idx_max);
  printf("\n");
  printf("#define FIXED_LIST \\\n");
  column_idx=print_string_as_u64(0,fixed_list_base,number_idx_max+1,1);
  column_idx=print_u64_string_end(column_idx,0,0);
  printf("\n");
  printf("#define CUSTOM_LIST \\\n");
  column_idx=print_string_as_i32(0,custom_list_base,number_idx_max+1);
  column_idx=print_i32_string_end(column_idx,0,0);
  printf("\n");
  printf("#define BINOMIAL_LIST \\\n");
  monomial_idx_bit_count=bit_count_get(monomial_idx_max);
  number_idx_bit_count=bit_count_get(number_idx_max);
  coeff_idx_bit_count=bit_count_get(header.coeff_idx_max);
/*
Account for the fact that the sign bit is stored in the LSB of each ratio_idx at binomial_list_base.
*/
  number_idx_bit_count++;
  column_idx=0;
  binomial_idx_read=0;
  packed_bit_count=0;
  transform_size=0;
  binomial_list_u64_count=0;
  packed_lo=0;
  packed_hi=0;
  do{
    value=binomial_list_base[binomial_idx_read].denominator_idx;
    status=pack(&packed_bit_count,&packed_hi,&packed_lo,value,monomial_idx_bit_count);
    if(status){
      transform_size+=U64_SIZE;
      packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
      column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
      binomial_list_u64_count++;
      column_idx=print_u64_string_end(column_idx,0,1);
    }
    value=binomial_list_base[binomial_idx_read].numerator_idx;
    status=pack(&packed_bit_count,&packed_hi,&packed_lo,value,monomial_idx_bit_count);
    if(status){
      transform_size+=U64_SIZE;
      packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
      column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
      binomial_list_u64_count++;
      column_idx=print_u64_string_end(column_idx,0,1);
    }
    value=binomial_list_base[binomial_idx_read].ratio_idx;
    binomial_idx_read++;
    status=pack(&packed_bit_count,&packed_hi,&packed_lo,value,number_idx_bit_count);
    if(status){
      transform_size+=U64_SIZE;
      packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
      column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
      binomial_list_u64_count++;
      if(packed_bit_count){
        column_idx=print_u64_string_end(column_idx,0,1);
      }else{
        column_idx=print_u64_string_end(column_idx,binomial_idx_read,binomial_count);
      }
    }
  }while(binomial_idx_read!=binomial_count);
  if(packed_bit_count){
    transform_size+=(packed_bit_count>>U8_BITS_LOG2)+!!(packed_bit_count&U8_BIT_MAX);
    packed_hi>>=U64_BITS-packed_bit_count;
    column_idx=print_string_as_u64(column_idx,&packed_hi,1,0);
    binomial_list_u64_count++;
    print_u64_string_end(column_idx,0,0);
  }
  printf("\n");
  printf("#define TRANSFORM_STRING \\\n");
  column_idx=0;
  transform_idx=0;
  packed_bit_count=0;
  transform_string_u64_count=0;
  packed_lo=0;
  packed_hi=0;
  do{
    monomial_count=transform_string_base[transform_idx];
    transform_idx++;
    status=pack(&packed_bit_count,&packed_hi,&packed_lo,monomial_count,monomial_count_bit_count);
    if(status){
      transform_size+=U64_SIZE;
      packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
      column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
      transform_string_u64_count++;
      column_idx=print_u64_string_end(column_idx,0,1);
    }
    monomial_count=transform_string_base[transform_idx];
    transform_idx++;
    status=pack(&packed_bit_count,&packed_hi,&packed_lo,monomial_count,monomial_count_bit_count);
    if(status){
      transform_size+=U64_SIZE;
      packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
      column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
      transform_string_u64_count++;
      column_idx=print_u64_string_end(column_idx,0,1);
    }
    while(monomial_count--){
      alias_count=transform_string_base[transform_idx];
      transform_idx++;
      status=pack(&packed_bit_count,&packed_hi,&packed_lo,alias_count,alias_count_bit_count);
      if(status){
        transform_size+=U64_SIZE;
        packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
        column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
        transform_string_u64_count++;
        column_idx=print_u64_string_end(column_idx,0,1);
      }
      while(alias_count--){
        value=transform_string_base[transform_idx];
        transform_idx++;
        status=pack(&packed_bit_count,&packed_hi,&packed_lo,value,number_idx_bit_count);
        if(status){
          transform_size+=U64_SIZE;
          packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
          column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
          transform_string_u64_count++;
          column_idx=print_u64_string_end(column_idx,0,1);
        }
        value=transform_string_base[transform_idx];
        status=pack(&packed_bit_count,&packed_hi,&packed_lo,value,coeff_idx_bit_count);
        if(status){
          transform_size+=U64_SIZE;
          packed_lo=pack_output(packed_bit_count,packed_hi,packed_lo);
          column_idx=print_string_as_u64(column_idx,&packed_lo,1,0);
          transform_string_u64_count++;
          if(packed_bit_count){
            column_idx=print_u64_string_end(column_idx,0,1);
          }else{
            column_idx=print_u64_string_end(column_idx,transform_idx,transform_idx_max);
          }
        }
        transform_idx++;
      }
    }
    transform_idx--;
  }while((transform_idx++)!=transform_idx_max);
  if(packed_bit_count){
    transform_size+=(packed_bit_count>>U8_BITS_LOG2)+!!(packed_bit_count&U8_BIT_MAX);
    packed_hi>>=U64_BITS-packed_bit_count;
    column_idx=print_string_as_u64(column_idx,&packed_hi,1,0);
    transform_string_u64_count++;
    print_u64_string_end(column_idx,0,0);
  }
  printf("\n");
  print_define_decimal("ALIAS_COUNT_BIT_COUNT",alias_count_bit_count);
  mask=mask_get(alias_count_bit_count);
  print_define("ALIAS_COUNT_MASK",mask);
  print_define("BINOMIAL_COUNT",header.binomial_idx_max+1);
  print_define("BINOMIAL_IDX_MAX",header.binomial_idx_max);
  print_define("BINOMIAL_LIST_U64_COUNT",binomial_list_u64_count);
  print_define("COEFF_COUNT",header.coeff_idx_max+1);
  print_define_decimal("COEFF_IDX_BIT_COUNT",coeff_idx_bit_count);
  mask=mask_get(coeff_idx_bit_count);
  print_define("COEFF_IDX_MASK",mask);
  print_define("COEFF_IDX_MAX",header.coeff_idx_max);
  print_define("MONOMIAL_COUNT",monomial_idx_max+1);
  print_define_decimal("MONOMIAL_COUNT_BIT_COUNT",monomial_count_bit_count);
  mask=mask_get(monomial_count_bit_count);
  print_define("MONOMIAL_COUNT_MASK",mask);
  print_define_decimal("MONOMIAL_IDX_BIT_COUNT",monomial_idx_bit_count);
  mask=mask_get(monomial_idx_bit_count);
  print_define("MONOMIAL_IDX_MASK",mask);
  print_define("MONOMIAL_IDX_MAX",monomial_idx_max);
  print_define("NUMBER_COUNT",number_count);
  print_define_decimal("NUMBER_IDX_BIT_COUNT",number_idx_bit_count);
  mask=mask_get(number_idx_bit_count);
  print_define("NUMBER_IDX_MASK",mask);
  print_define("NUMBER_IDX_MAX",number_idx_max);
  number_list_size=number_count*sizeof(NUMBER_OUT);
  print_define("NUMBER_LIST_SIZE",number_list_size);
  print_define("ORTHOHACK_DEFINED",1);
  print_define_decimal("PRECISION_CUSTOM",PRECISION_CUSTOM);
  print_define_decimal("PRECISION_FIXED",PRECISION_FIXED);
  print_define_decimal("PRECISION_INTERNAL",PRECISION_OUT);
  print_define("SAMPLE_IDX_MAX",header.sample_idx_max);
  print_define("TRANSFORM_IDX_MAX",transform_idx_max);
  print_define("TRANSFORM_STRING_U64_COUNT",transform_string_u64_count);
  printf("/*\n");
  DEBUG_U64("transform_size",transform_size);
  string_free(custom_list_base);
  string_free(fixed_list_base);
  string_free(number_list_base);
  string_free(sort_tag_list1_base);
  string_free(sort_tag_list0_base);
  string_free(number_list_optimized_base);
  sort_free(sort_string_base);
  string_free(number_list_consolidated_base);
  string_free(alias_idx_list_base);
  string_free(transform_string_base);
  string_free(alias_list_base);
  string_free(monomial_count_list_base);
  string_list_free(coeff_count,(u8 **)(monomial_base_list_base));
  if(fixed_list_invalid_status){
    printf("WARNING: FIXED_LIST is invalid, probably due to integer overflow, meaning that\nfixed-point transforms won't work.\n");
  }
  if(custom_list_invalid_status){
    printf("WARNING: CUSTOM_LIST is invalid, probably due to integer overflow, meaning that\ncustom fixed-point transforms won't work. You might be able to fix this by\nreducing CUSTOM_FRACTION_BIT_COUNT.\n");
  }
  printf("Done.\n*/\n");
  return 0;
}
