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
void
number_list_sort(u64 *sort_string_base,u64 number_idx_max,sort_tag_t *sort_tag_list0_base,sort_tag_t *sort_tag_list1_base,NUMBER *number_list_in_base,NUMBER *number_list_out_base){
/*
In:

  sort_string_base is the return value of sort_init().

  number_idx_max is the maximum (number index).

  *sort_tag_list0_base is undefined and writable for (number_idx_max+1)*sizeof(sort_tag_t).

  *sort_tag_list1_base is undefined and writable for (number_idx_max+1)*sizeof(sort_tag_t).

  *number_list_in_base is the list of (NUMBER)s to sort. (NaN)s are not allowed, but infinities, denormals, and negative zeros are OK. Negative zero is considered equal to zero, and as such, these could appear in random order with respect to each other.

  *number_list_out_base is undefined and writable for the size of *number_list_in_base.

Out:

  *sort_tag_list0_base is undefined.

  *sort_tag_list1_base is undefined.

  *number_list_out_base is the negative-to-positive sorted equivalent of *number_list_in_base.
*/
  u64 fraction;
  NUMBER number0;
  NUMBER number1;
  u64 number_idx;
  NUMBER number_max;
  NUMBER number_min;
  NUMBER number_scaler;
  NUMBER number_span;
  u8 sorted_status;
/*
Start out moving backwards because *number_list_in_base was probably just written from start to end, leaving the end in the cache.
*/
  number_min=number_list_in_base[number_idx_max];
  number_max=number_min;
  for(number_idx=number_idx_max-1;number_idx<number_idx_max;number_idx--){
    number0=number_list_in_base[number_idx];
    if(IS_LESS(number0,number_min)){
      number_min=number0;
    }else if(IS_GREATER(number0,number_max)){
      number_max=number0;
    }
  }
/*
Scale all the (NUMBER)s so they fit somewhere on [0,U64_MAX]. We actually do this by scaling them to [U32_MAX,U64_MAX-U32_MAX] in order to more than compensate for the maximum possible numerical error, so that we are guaranteed not to wrap. (Wrapping still wouldn't result in a wrong output, but it would have catastrophic timing effects on the bubble sort below.)

After the above is done, we can sort the (NUMBER)s by attaching them to the sort tags of their integer equivalents. There's a small chance that the sort will be wrong in a few places due to precision issues, which we'll fix below with a fast bubble sort.
*/
  number_span=DIFFERENCE(number_max,number_min);
  number_scaler=RATIO(NUMBER_MAKE(U64_MAX-U32_MAX-U32_MAX),number_span);
  for(number_idx=0;number_idx<=number_idx_max;number_idx++){
    number0=number_list_in_base[number_idx];
    fraction=SUM(PRODUCT(DIFFERENCE(number0,number_min),number_scaler),NUMBER_MAKE(U32_MAX));
    sort_tag_list0_base[number_idx].item=fraction;
    sort_tag_list0_base[number_idx].struct_idx=number_idx;
  }
  sort_tag_list_sort(sort_string_base,sizeof(NUMBER),(u8 *)(number_list_in_base),(u8 *)(number_list_out_base),number_idx_max,&sort_tag_list0_base,&sort_tag_list1_base);
/*
Everything is probably sorted correctly, but it there might be a few inversions here and there due to precision loss during integer conversion above. Iteratively bubble sort until everything is in order.

Alternate backwards and forwards sweeps in order to maximize cache hits.
*/
  do{
    sorted_status=1;
    for(number_idx=number_idx_max-1;number_idx<number_idx_max;number_idx--){
      number0=number_list_out_base[number_idx];
      number1=number_list_out_base[number_idx+1];
      if(IS_GREATER(number0,number1)){
        number_list_out_base[number_idx]=number1;
        number_list_out_base[number_idx+1]=number0;
        sorted_status=0;
      }
    }
    if(!sorted_status){
      sorted_status=1;
      for(number_idx=0;number_idx<number_idx_max;number_idx++){
        number0=number_list_out_base[number_idx];
        number1=number_list_out_base[number_idx+1];
        if(IS_GREATER(number0,number1)){
          number_list_out_base[number_idx]=number1;
          number_list_out_base[number_idx+1]=number0;
          sorted_status=0;
        }
      }
    }
  }while(!sorted_status);
  return;
}

u64
number_list_consolidate(u64 *sort_string_base,sort_tag_t *sort_tag_list0_base,sort_tag_t *sort_tag_list1_base,NUMBER *number_list_in_base,NUMBER *number_list_out_base,u64 *number_idx_max_base,NUMBER *ratio_pop_max_base){
/*
In:

  *sort_tag_list0_base is as defined in number_list_sort():In.

  *sort_tag_list1_base is as defined in number_list_sort():In.

Out:

  *sort_tag_list0_base is undefined.

  *sort_tag_list1_base is undefined.

  *ratio_pop_max_base is the value of the most populous nonzero NUMBER ((NUMBER)s within PRECISION_RATIO_LIMIT being considered equal) if it exists, else 0.0.
*/
  u8 distinct_status;
  u64 idx_read;
  u64 idx_read_min;
  u64 idx_write;
  u64 number_idx_max;
  NUMBER number0;
  NUMBER number1;
  u64 population;
  u64 population_max;
  NUMBER ratio;
  NUMBER ratio_pop_max;
  NUMBER sum;

  number_idx_max=*number_idx_max_base;
  number_list_sort(sort_string_base,number_idx_max,sort_tag_list0_base,sort_tag_list1_base,number_list_in_base,number_list_out_base);
  distinct_status=0;
  number0=*number_list_out_base;
  sum=number0;
  idx_read_min=0;
  idx_read=1;
  idx_write=0;
  population_max=0;
  SET_ZERO(ratio_pop_max);
  while(idx_read<=number_idx_max){
    number1=number_list_out_base[idx_read];
    idx_read++;
    distinct_status=1;
    if(IS_NONNEGATIVE(number0)){
      if(IS_NONNEGATIVE(number1)){
        if(IS_LESS_EQUAL(number0,PRODUCT(NUMBER_MAKE(PRECISION_RATIO_LIMIT),number1))){
          if(IS_LESS_EQUAL(number1,PRODUCT(NUMBER_MAKE(PRECISION_RATIO_LIMIT),number0))){
            distinct_status=0;
          }
        }
      }
    }else if(IS_NEGATIVE(number0)){
      if(IS_NEGATIVE(number1)){
        if(IS_GREATER_EQUAL(number0,PRODUCT(NUMBER_MAKE(PRECISION_RATIO_LIMIT),number1))){
          if(IS_GREATER_EQUAL(number1,PRODUCT(NUMBER_MAKE(PRECISION_RATIO_LIMIT),number0))){
            distinct_status=0;
          }
        }
      }
    }
    if(!distinct_status){
      sum=SUM(sum,number1);
    }else{
      population=idx_read-1-idx_read_min;
      ratio=RATIO(sum,NUMBER_MAKE(population));
      number_list_out_base[idx_write]=ratio;
      if(population>population_max){
        if(ratio){
          population_max=population;
          ratio_pop_max=ratio;
        }
      }
      idx_write++;
      idx_read_min=idx_read-1;
      number0=number1;
      sum=number1;
    }
  }
  population=idx_read-idx_read_min;
  ratio=RATIO(sum,NUMBER_MAKE(population));
  number_list_out_base[idx_write]=ratio;
  if(population>population_max){
    if(ratio){
      population_max=population;
      ratio_pop_max=ratio;
    }
  }
  *number_idx_max_base=idx_write;
  *ratio_pop_max_base=ratio_pop_max;
  return population_max;
}

NUMBER
number_list_find_nearest(NUMBER number0,u64 number_idx_max,NUMBER *number_list_sorted_base){
  NUMBER number1;
  NUMBER number2;
  u64 number_idx_mean;
  u64 number_idx_min;
  NUMBER number_max;
  NUMBER number_min;

  number1=*number_list_sorted_base;
  if(IS_GREATER(number0,number1)){
    number1=number_list_sorted_base[number_idx_max];
    if(IS_LESS(number0,number1)){
      number_idx_min=0;
/*

(number_idx_min!=number_idx_max). Find the NUMBER at number_list_sorted_base which is closest to number0.
*/
      do{
        number_idx_mean=number_idx_max-((number_idx_max-number_idx_min)>>1);
        number1=number_list_sorted_base[number_idx_mean];
        if(IS_LESS(number0,number1)){
          number_idx_max=number_idx_mean-1;
        }else{
          number_idx_min=number_idx_mean;
        }
      }while(number_idx_min!=number_idx_max);
/*
(number_list_sorted_base[number_idx_min]<=number0), and (number0<number_list_consolidated_base[number_idx_min+1]). So now just test the (NUMBER)s at [number_idx_min] and [number_idx_min+1] to see which is closer, and go with it. There are some esoteric reasons to suggest that we should consider "closer" to mean "closer in a ratio sense to 1". But then we get a singularity at 0, and frankly if 2 values are so close together that it would make a difference, then either they are effectively interchangeable, or we are operating at very low precision and don't really care anyway.
*/
      number1=number_list_sorted_base[number_idx_min];
      number2=number_list_sorted_base[number_idx_min+1];
      if(IS_LESS(DIFFERENCE(number2,number0),DIFFERENCE(number0,number1))){
        number1=number2;
      }
    }
  }
  return number1;
}

u64
number_list_find_nearest_idx(NUMBER number0,u64 number_idx_max,NUMBER *number_list_sorted_base){
  NUMBER number1;
  NUMBER number2;
  u64 number_idx_mean;
  u64 number_idx_min;
  NUMBER number_max;
  NUMBER number_min;

  number1=*number_list_sorted_base;
  number_idx_min=0;
  if(IS_GREATER(number0,number1)){
    number1=number_list_sorted_base[number_idx_max];
    number_idx_min=number_idx_max;
    if(IS_LESS(number0,number1)){
      number_idx_min=0;
/*

(number_idx_min!=number_idx_max). Find the NUMBER at number_list_sorted_base which is closest to number0.
*/
      do{
        number_idx_mean=number_idx_max-((number_idx_max-number_idx_min)>>1);
        number1=number_list_sorted_base[number_idx_mean];
        if(IS_LESS(number0,number1)){
          number_idx_max=number_idx_mean-1;
        }else{
          number_idx_min=number_idx_mean;
        }
      }while(number_idx_min!=number_idx_max);
/*
(number_list_sorted_base[number_idx_min]<=number0), and (number0<number_list_consolidated_base[number_idx_min+1]). So now just test the (NUMBER)s at [number_idx_min] and [number_idx_min+1] to see which is closer, and go with it. There are some esoteric reasons to suggest that we should consider "closer" to mean "closer in a ratio sense to 1". But then we get a singularity at 0, and frankly if 2 values are so close together that it would make a difference, then either they are effectively interchangeable, or we are operating at very low precision and don't really care anyway.
*/
      number1=number_list_sorted_base[number_idx_min];
      number2=number_list_sorted_base[number_idx_min+1];
      if(IS_LESS(DIFFERENCE(number2,number0),DIFFERENCE(number0,number1))){
        number_idx_min++;
      }
    }
  }
  return number_idx_min;
}

void
number_list_sign_zero(u64 number_count_minus_1,NUMBER *number_list_base,u8 *sign_bitmap_base){
  u64 idx;
  NUMBER number0;

  idx=0;
  do{
    number0=number_list_base[idx];
    if(number0>=0.0f){
      BIT_CLEAR(sign_bitmap_base,idx);
    }else{
      number_list_base[idx]=-number0;
      BIT_SET(sign_bitmap_base,idx);
    }
    idx++;
  }while(idx<=number_count_minus_1);
  return;
}

void
number_list_sign_restore(u64 number_count_minus_1,NUMBER *number_list_base,u8 *sign_bitmap_base){
/*
Restore signs to a list of (NUMBER)s.
*/
  u64 idx;
  NUMBER number0;

  idx=0;
  do{
    number0=number_list_base[idx];
    if(BIT_TEST(sign_bitmap_base,idx)){
      number_list_base[idx]=-number0;
    }
    idx++;
  }while(idx<=number_count_minus_1);
  return;
}

u64
number_list_flush_zero(u64 number_idx_max,NUMBER *number_list_base){
  u64 nonzero_sample_count;
  u64 number_idx;
  NUMBER number0;

  number_idx=0;
  nonzero_sample_count=number_idx_max+1;
  do{
    number0=number_list_base[number_idx];
    if(IS_LESS_EQUAL(MAGNITUDE(number0),NUMBER_MAKE(PRECISION_ZERO_LIMIT))){
      SET_ZERO(number_list_base[number_idx]);
      nonzero_sample_count--;
    }
    number_idx++;
  }while(number_idx<=number_idx_max);
  return nonzero_sample_count;
}

u64
number_list_consolidated_flush_zero_one(u64 number_idx_max,NUMBER *number_list_consolidated_base){
/*
Flush neighbors of -1,0 and 1.0, to -1.0 and 1.0, respectively. Ensure that neither value, nor 0.0, occurs more than once in the output. -0.0 will be flushed to 0.0. Note that this is different behavior than number_list_flush_zero(), because this function is intended to preserve tiny ratios, as opposed to zeroing tiny samples.

In:

  *number_list_consolidated_base contains (number_idx_max+1) (NUMBER)s, sorted from most to least negative.
*/
  NUMBER magnitude;
  NUMBER number;
  u64 number_idx_read;
  u64 number_idx_write;
  u8 path;

  number_idx_read=0;
  number_idx_write=0;
  path=3;
  do{
    number=number_list_consolidated_base[number_idx_read];
    magnitude=MAGNITUDE(number);
    if(IS_ZERO(magnitude)){
/*
Don't store more than one 0.0.
*/
      if(path){
/*
Store -0.0 as 0.0.
*/
        number_list_consolidated_base[number_idx_write]=magnitude;
        number_idx_write++;
      }
      path=0;
    }else if((IS_LESS_EQUAL(magnitude,NUMBER_MAKE(PRECISION_RATIO_LIMIT)))&&(IS_LESS_EQUAL(NUMBER_MAKE(1.0f),PRODUCT(NUMBER_MAKE(PRECISION_RATIO_LIMIT),magnitude)))){
/*
Flush PRECISION_RATIO_LIMIT neighbors of +/-1.0 to +/-1.0, but don't store more than one -1.0 and one 1.0.
*/
      if(IS_NONNEGATIVE(number)){
        if(path!=1){
          SET_ONE(number);
          number_list_consolidated_base[number_idx_write]=number;
          number_idx_write++;
        }
        path=1;
      }else{
        if(path!=2){
          SET_NEGATIVE_ONE(number);
          number_list_consolidated_base[number_idx_write]=number;
          number_idx_write++;
        }
        path=2;
      }      
    }else{
/*
Copy this number unchanged.
*/
      number_list_consolidated_base[number_idx_write]=number;
      number_idx_write++;
      path=3;
    }
  }while((number_idx_read++)!=number_idx_max);
  return number_idx_write-1;
}
