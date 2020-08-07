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
#include "pre.h"
#include "orthodim.h"
#include "orthonumber.h"
#include "number_io.h"
#include "tree.h"
#include "ohk_header.h"
#include "print.c"
#include "debug.c"
#include "string.c"
#include "spawn.c"
#include "sort.c"
#include "file_sys.c"
#include "number.c"

TYPEDEF_START
  binomial_t *binomial_list_base;
  u64 denominator_idx_min_minus_1;
  i64 *population_list_base;
  NUMBER **ratio_list_base_list0_base;
  NUMBER **ratio_list_base_list1_base;
  NUMBER *sample_list_base;
  u64 **sort_string_base_list_base;
  sort_tag_t **sort_tag_list_base_list0_base;
  sort_tag_t **sort_tag_list_base_list1_base;
  u8 population_negation_status;
TYPEDEF_END(binomial_spawn_t)

void
binomial_populest_get(u64 numerator_idx_max,NUMBER *sample_list_base,u64 *sort_string_base,sort_tag_t *sort_tag_list0_base,sort_tag_t *sort_tag_list1_base,NUMBER *ratio_list0_base,NUMBER *ratio_list1_base,binomial_t *binomial_base,i64 *population_base){
  u64 denominator_idx;
  u64 denominator_idx_linear;
  u64 denominator_idx_linear_max;
  u64 denominator_idx_linear_min;
  u64 ignored;
  u64 numerator_idx;
  u64 numerator_idx_pop_max;
  u64 numerator_idx_linear;
  i64 population;
  i64 population_max;
  NUMBER ratio0;
  NUMBER ratio1;
  u64 ratio_idx;
  NUMBER ratio_pop_max;
  u64 sample_idx;

  denominator_idx=numerator_idx_max+1;
  denominator_idx_linear_min=denominator_idx*COEFF_COUNT;
  denominator_idx_linear_max=denominator_idx_linear_min+COEFF_COUNT-1;
  numerator_idx=0;
  numerator_idx_linear=0;
  numerator_idx_pop_max=0;
  population_max=0;
  SET_ZERO(ratio_pop_max);
  do{
    denominator_idx_linear=denominator_idx_linear_min;
    ratio_idx=0;
    for(;denominator_idx_linear<=denominator_idx_linear_max;denominator_idx_linear++){
      ratio0=sample_list_base[denominator_idx_linear];
      ratio1=sample_list_base[numerator_idx_linear];
      numerator_idx_linear++;
/*
We're not intersted in ratios of 0.0 or infinity, which would occur often if we didn't check.
*/
      if(IS_NONZERO(ratio0)&&IS_NONZERO(ratio1)){
        ratio0=RATIO(ratio1,ratio0);
        ratio_list0_base[ratio_idx]=ratio0;
        ratio_idx++;
      }
    }
    if(ratio_idx){
      ratio_idx--;
      population=number_list_consolidate(sort_string_base,sort_tag_list0_base,sort_tag_list1_base,ratio_list0_base,ratio_list1_base,&ratio_idx,&ratio0);
      if(population>population_max){
        population_max=population;
        numerator_idx_pop_max=numerator_idx;
        ratio_pop_max=ratio0;
      }
    }
  }while((numerator_idx++)!=numerator_idx_max);
  *population_base=population_max;
  binomial_base->numerator_idx=numerator_idx_pop_max;
  binomial_base->denominator_idx=denominator_idx;
  binomial_base->ratio=ratio_pop_max;
  return;
}

void
binomial_populest_get_spawn(thread_context_t *thread_context_base){
  binomial_spawn_t *binomial_spawn_base;
  binomial_t *binomial_list_base;
  u64 numerator_idx_max;
  i64 *population_list_base;
  NUMBER *ratio_list0_base;
  NUMBER *ratio_list1_base;
  NUMBER *sample_list_base;
  u64 *sort_string_base;
  sort_tag_t *sort_tag_list0_base;
  sort_tag_t *sort_tag_list1_base;
  u64 simulthread_idx;

  simulthread_idx=thread_context_base->simulthread_idx;
  binomial_spawn_base=(binomial_spawn_t *)(thread_context_base->readonly_string_base);
  numerator_idx_max=thread_context_base->thread_idx;
  numerator_idx_max+=binomial_spawn_base->denominator_idx_min_minus_1;
  binomial_list_base=binomial_spawn_base->binomial_list_base;
  population_list_base=binomial_spawn_base->population_list_base;
  ratio_list0_base=binomial_spawn_base->ratio_list_base_list0_base[simulthread_idx];
  ratio_list1_base=binomial_spawn_base->ratio_list_base_list1_base[simulthread_idx];
  sample_list_base=binomial_spawn_base->sample_list_base;
  sort_string_base=binomial_spawn_base->sort_string_base_list_base[simulthread_idx];
  sort_tag_list0_base=binomial_spawn_base->sort_tag_list_base_list0_base[simulthread_idx];
  sort_tag_list1_base=binomial_spawn_base->sort_tag_list_base_list1_base[simulthread_idx];
  binomial_populest_get(numerator_idx_max,sample_list_base,sort_string_base,sort_tag_list0_base,sort_tag_list1_base,ratio_list0_base,ratio_list1_base,&binomial_list_base[numerator_idx_max],&population_list_base[numerator_idx_max]);
  if(binomial_spawn_base->population_negation_status){
/*
Negate the population as an indicator that it has been refreshed.
*/
    population_list_base[numerator_idx_max]=-population_list_base[numerator_idx_max];
  }
  return;
}

u8
binomial_list_get(u64 denominator_idx_min_minus_1,u64 sample_idx_max_minus_1,NUMBER *sample_list_base,u64 *binomial_count_base,binomial_t *binomial_list_base,i64 *population_list_base,u64 *population_max_base){
  u64 binomial_count;
  u64 binomial_count_max;
  binomial_t *binomial_list_in_base;
  binomial_t *binomial_list_out_base;
  u64 binomial_read_idx;
  binomial_spawn_t binomial_spawn;
  u64 binomial_write_idx;
  u64 denominator_idx;
  u64 denominator_idx_minus_1;
  u64 numerator_idx;
  NUMBER one;
  i64 population;
  i64 population_max;
  i64 population_max_negative;
  u8 *sample_idx_used_bitmap_base;
  u64 sample_idx_used_bitmap_size;
  u64 simulthread_idx;
  NUMBER skew;
  u64 skew_idx;
  NUMBER *skew_list_base;
  NUMBER *skew_list_sorted_base;
  u64 *sort_string_base;
  sort_tag_t *sort_tag_list0_base;
  sort_tag_t *sort_tag_list1_base;
  u8 status;
  thread_set_t *thread_set_base;

  binomial_spawn.sort_string_base_list_base=(u64 **)(string_item_list_create(SIMULTHREAD_IDX_MAX+1,sizeof(u64 *)));
  status=!binomial_spawn.sort_string_base_list_base;
  if(!status){
    simulthread_idx=0;
    do{
      sort_string_base=sort_init();
      binomial_spawn.sort_string_base_list_base[simulthread_idx]=sort_string_base;
      status|=!sort_string_base;
    }while((simulthread_idx++)!=SIMULTHREAD_IDX_MAX);
    binomial_spawn.binomial_list_base=binomial_list_base;
    binomial_spawn.population_list_base=population_list_base;
    binomial_spawn.ratio_list_base_list0_base=(NUMBER **)(string_list_create(SIMULTHREAD_IDX_MAX+1,COEFF_COUNT*sizeof(NUMBER)));
    status|=!binomial_spawn.ratio_list_base_list0_base;
    binomial_spawn.ratio_list_base_list1_base=(NUMBER **)(string_list_create(SIMULTHREAD_IDX_MAX+1,COEFF_COUNT*sizeof(NUMBER)));
    status|=!binomial_spawn.ratio_list_base_list1_base;
    binomial_spawn.sample_list_base=sample_list_base;
    binomial_spawn.sort_tag_list_base_list0_base=(sort_tag_t **)(string_list_create(SIMULTHREAD_IDX_MAX+1,COEFF_COUNT*sizeof(sort_tag_t)));
    status|=!binomial_spawn.sort_tag_list_base_list0_base;
    binomial_spawn.sort_tag_list_base_list1_base=(sort_tag_t **)(string_list_create(SIMULTHREAD_IDX_MAX+1,COEFF_COUNT*sizeof(sort_tag_t)));
    status|=!binomial_spawn.sort_tag_list_base_list1_base;
    thread_set_base=SPAWN_INIT(&binomial_populest_get_spawn,(u8 *)(&binomial_spawn),SIMULTHREAD_IDX_MAX);
    status|=!thread_set_base;
    if(!status){
      binomial_spawn.denominator_idx_min_minus_1=denominator_idx_min_minus_1;
      binomial_spawn.population_negation_status=0;
      SPAWN(sample_idx_max_minus_1-denominator_idx_min_minus_1,thread_set_base);
      binomial_spawn.denominator_idx_min_minus_1=0;
      binomial_spawn.population_negation_status=1;
      do{
        population_max=0;
        population_max_negative=0;
        for(denominator_idx_minus_1=sample_idx_max_minus_1;denominator_idx_minus_1<=sample_idx_max_minus_1;denominator_idx_minus_1--){
          population=population_list_base[denominator_idx_minus_1];
          if(population>population_max){
            population_max=population;
          }else if(population<population_max_negative){
            population_max_negative=population;
          }
        }
        population_max_negative=-population_max_negative;
        if(population_max_negative>population_max){
          break;
        }
        if(!(population_max|population_max_negative)){
          break;
        }
/*
Reevaluate any population==population_max, because it might have decreased due to substitutions.
*/
        SPAWN_REWIND(&binomial_populest_get_spawn,(u8 *)(&binomial_spawn),thread_set_base);
        for(denominator_idx_minus_1=0;denominator_idx_minus_1<=sample_idx_max_minus_1;denominator_idx_minus_1++){
          population=population_list_base[denominator_idx_minus_1];
          if(population==population_max){
            status=SPAWN_ONE(thread_set_base,denominator_idx_minus_1);
            if(status){
              break;
            }
          }
        }
        SPAWN_RETIRE_ALL(thread_set_base);
      }while(!status);
      population_max=population_max_negative;
    }
    SPAWN_FREE(thread_set_base);
    string_list_free(SIMULTHREAD_IDX_MAX+1,(u8 **)(binomial_spawn.sort_tag_list_base_list1_base));
    string_list_free(SIMULTHREAD_IDX_MAX+1,(u8 **)(binomial_spawn.sort_tag_list_base_list0_base));
    string_list_free(SIMULTHREAD_IDX_MAX+1,(u8 **)(binomial_spawn.ratio_list_base_list1_base));
    string_list_free(SIMULTHREAD_IDX_MAX+1,(u8 **)(binomial_spawn.ratio_list_base_list0_base));
    simulthread_idx=SIMULTHREAD_IDX_MAX;
    do{
      sort_free(binomial_spawn.sort_string_base_list_base[simulthread_idx]);
    }while(simulthread_idx--);
    string_free(binomial_spawn.sort_string_base_list_base);
    sample_idx_used_bitmap_base=string_bitmap_create(sample_idx_max_minus_1+2,1,0,&sample_idx_used_bitmap_size);
    status|=!sample_idx_used_bitmap_base;
    sort_string_base=sort_init();
    status|=!sort_string_base;
    binomial_count_max=sample_idx_max_minus_1+1;
    sort_tag_list0_base=(sort_tag_t *)(string_item_list_create(binomial_count_max,sizeof(sort_tag_t)));
    status|=!sort_tag_list0_base;
    sort_tag_list1_base=(sort_tag_t *)(string_item_list_create(binomial_count_max,sizeof(sort_tag_t)));
    status|=!sort_tag_list1_base;
    skew_list_base=(NUMBER *)(string_item_list_create(binomial_count_max,sizeof(NUMBER)));
    status|=!skew_list_base;
    skew_list_sorted_base=(NUMBER *)(string_item_list_create(binomial_count_max,sizeof(NUMBER)));
    status|=!skew_list_sorted_base;
    binomial_list_in_base=(binomial_t *)(string_item_list_create(binomial_count_max,sizeof(binomial_t)));
    status|=!binomial_list_in_base;
    binomial_list_out_base=(binomial_t *)(string_item_list_create(binomial_count_max,sizeof(binomial_t)));
    status|=!binomial_list_out_base;
    if(!status){
      *population_max_base=population_max;
      binomial_count=0;
      SET_ONE(one);
      if(population_max){
        binomial_write_idx=0;
        for(denominator_idx_minus_1=0;denominator_idx_minus_1<=sample_idx_max_minus_1;denominator_idx_minus_1++){
          population=population_list_base[denominator_idx_minus_1];
          if(population<0){
            population=-population;
            population_list_base[denominator_idx_minus_1]=population;
          }
          if(population==population_max){
/*
Compute the skew of the numerator/denominator ratio, namely, the magnitude of (its magnitude minus 1). We do this because we want to preferentially select binomials who ratio are closer to 1, becaused fixed point transforms benefit from narrow dynamic range. (In the (likely) event that not all numerators and denominators are orthogonal, it would be preferable to drop only the (binomial_t)s with highest (skew)s. Empirically, the resulting transforms also have lower computational complexity, for reasons not immediately evident.
*/
            skew=binomial_list_base[denominator_idx_minus_1].ratio;
            skew=MAGNITUDE(DIFFERENCE(MAGNITUDE(skew),one));
            skew_list_base[binomial_write_idx]=skew;
            binomial_write_idx++;
          }
        }
        binomial_count=binomial_write_idx;
        binomial_write_idx--;
        number_list_sort(sort_string_base,binomial_write_idx,sort_tag_list0_base,sort_tag_list1_base,skew_list_base,skew_list_sorted_base);
        for(denominator_idx_minus_1=sample_idx_max_minus_1;denominator_idx_minus_1<=sample_idx_max_minus_1;denominator_idx_minus_1--){
          population=population_list_base[denominator_idx_minus_1];
          if(population==population_max){
            skew=binomial_list_base[denominator_idx_minus_1].ratio;
            skew=MAGNITUDE(DIFFERENCE(MAGNITUDE(skew),one));
/*
We expect to find skew somewhere in *skew_list_sorted_base. However, issues with 80-bit caching of 64-bit (double)s on X86 (but not X64) with GCC, can cause skew to differ from its memory image, and thus fail a test for equality. So instead of a fast search for an exact match, we need to implement a slow search for the closest match. The same caching issue could potentially occur on other architectures as well.
*/
            skew_idx=number_list_find_nearest_idx(skew,binomial_count-1,skew_list_sorted_base);
            sort_tag_list0_base[binomial_write_idx].item=skew_idx;
            binomial_list_in_base[binomial_write_idx]=binomial_list_base[denominator_idx_minus_1];
            sort_tag_list0_base[binomial_write_idx].struct_idx=binomial_write_idx;
            binomial_write_idx--;
          }
        }
/*
Sort the (binomial_t)s with population==population_max, ascending by skew.
*/
        sort_tag_list_sort(sort_string_base,sizeof(binomial_t),(u8 *)(binomial_list_in_base),(u8 *)(binomial_list_out_base),binomial_count-1,&sort_tag_list0_base,&sort_tag_list1_base);
/*
Scan through *binomial_list_out_base, looking for only those binomials with maximum population. We use a bitmap to ensure that all the "populest" binomials that we find are orthogonal, because we don't want conflicting substitutions.
*/
        binomial_write_idx=0;
        for(binomial_read_idx=0;binomial_read_idx<binomial_count;binomial_read_idx++){
          denominator_idx=binomial_list_out_base[binomial_read_idx].denominator_idx;
          if(BIT_TEST(sample_idx_used_bitmap_base,denominator_idx)==0){
            numerator_idx=binomial_list_out_base[binomial_read_idx].numerator_idx;
            if(BIT_TEST(sample_idx_used_bitmap_base,numerator_idx)==0){
              BIT_SET(sample_idx_used_bitmap_base,numerator_idx);
              binomial_list_base[binomial_write_idx].numerator_idx=numerator_idx;
              BIT_SET(sample_idx_used_bitmap_base,denominator_idx);
              binomial_list_base[binomial_write_idx].denominator_idx=denominator_idx;
              binomial_list_base[binomial_write_idx].ratio=binomial_list_out_base[binomial_read_idx].ratio;
              binomial_write_idx++;
            }
          }
        }
        binomial_count=binomial_write_idx;
      }
      *binomial_count_base=binomial_count;
    }
    string_free(binomial_list_out_base);
    string_free(binomial_list_in_base);
    string_free(skew_list_sorted_base);
    string_free(skew_list_base);
    string_free(sort_tag_list1_base);
    string_free(sort_tag_list0_base);
    sort_free(sort_string_base);
    string_free(sample_idx_used_bitmap_base);
  }
  return status;
}

u8
binomial_list_merge(u64 population_minus_2,i64 *population_list_base,u64 *binomial_count0_base,binomial_t *binomial_list0_base,u64 binomial_idx1_max,binomial_t *binomial_list1_base,NUMBER *sample_list_base){
  u64 binomial_count0;
  u64 binomial_idx1;
  binomial_t *binomial_list0_post;
  u64 denominator_idx;
  u64 denominator_idx_linear;
  u64 denominator_idx_linear_max;
  u64 numerator_idx;
  u64 numerator_idx_linear;
  u64 population;
  u64 ratio_idx;
  u64 ratio_idx_max;
  NUMBER *ratio_list0_base;
  NUMBER *ratio_list1_base;
  NUMBER *ratio_list2_base;
  NUMBER ratio0;
  NUMBER ratio1;
  u64 sample_idx_linear;
  u64 sample_idx_max;
  u64 *sort_string_base;
  sort_tag_t *sort_tag_list0_base;
  sort_tag_t *sort_tag_list1_base;
  u8 status;
  u64 substitution_count;

  binomial_count0=*binomial_count0_base;
  ratio_list0_base=(NUMBER *)(string_create(COEFF_COUNT*sizeof(NUMBER)));
  status=!ratio_list0_base;
  ratio_list1_base=(NUMBER *)(string_create(COEFF_COUNT*sizeof(NUMBER)));
  status|=!ratio_list1_base;
  ratio_list2_base=(NUMBER *)(string_create(COEFF_COUNT*sizeof(NUMBER)));
  status|=!ratio_list2_base;
  sort_string_base=sort_init();
  status|=!sort_string_base;
  sort_tag_list0_base=(sort_tag_t *)(string_create(COEFF_COUNT*sizeof(sort_tag_t)));
  status|=!sort_tag_list0_base;
  sort_tag_list1_base=(sort_tag_t *)(string_create(COEFF_COUNT*sizeof(sort_tag_t)));
  status|=!sort_tag_list1_base;
  if(!status){
    sample_idx_max=SAMPLE_COUNT+binomial_count0;
    sample_idx_linear=sample_idx_max*COEFF_COUNT;
    binomial_idx1=0;
/*
All binomials that we merge are expected to have the same population, which serves as a sanity check.
*/
    population=population_minus_2+2;
    do{
      numerator_idx=binomial_list1_base[binomial_idx1].numerator_idx;
      denominator_idx=binomial_list1_base[binomial_idx1].denominator_idx;
      numerator_idx_linear=numerator_idx*COEFF_COUNT;
      denominator_idx_linear=denominator_idx*COEFF_COUNT;
      denominator_idx_linear_max=denominator_idx_linear+COEFF_COUNT-1;
      ratio_idx=0;
      for(;denominator_idx_linear<=denominator_idx_linear_max;denominator_idx_linear++){
        ratio0=sample_list_base[denominator_idx_linear];
        ratio1=sample_list_base[numerator_idx_linear];
        numerator_idx_linear++;
/*
We're not intersted in ratios of 0.0 or infinity, which would occur often if we didn't check.
*/
        if(IS_NONZERO(ratio0)&&IS_NONZERO(ratio1)){
          ratio0=RATIO(ratio1,ratio0);
        }else{
/*
We still need to store a 0, even though it's a useless term, because we need to know the exact index at which to perform a binomial substitution, below.
*/
          SET_ZERO(ratio0);
        }
        ratio_list0_base[ratio_idx]=ratio0;
        ratio_idx++;
      }    
/*
Copy COEFF_COUNT (NUMBER)s from *ratio_list1_base to *ratio_list0_base so that we won't need to recompute everything after number_list_consolidate() destroys *ratio_list0_base and *ratio_list1_base.
*/
      memcpy((void *)(ratio_list2_base),(const void *)(ratio_list0_base),COEFF_COUNT*sizeof(NUMBER));
/*
We need to consolidate *ratio_list0_base. Otherwise we might not find as many substitutions as expected (population).
*/
      ratio_idx_max=COEFF_COUNT-1;
      number_list_consolidate(sort_string_base,sort_tag_list0_base,sort_tag_list1_base,ratio_list0_base,ratio_list1_base,&ratio_idx_max,&ratio0);
/*
Scan through *ratio_list2_base to find ratios which consolidate, via number_list_find_nearest(), to ratio0, which is the most populous ratio.
*/
      numerator_idx_linear-=COEFF_COUNT;
      denominator_idx_linear-=COEFF_COUNT;
      ratio_idx=0;
      substitution_count=0;
      for(;denominator_idx_linear<=denominator_idx_linear_max;denominator_idx_linear++){
        ratio1=ratio_list2_base[ratio_idx];
        ratio_idx++;
        if(ratio1){
/*
Replace ratio1 with its nearest neighbor from the consolidated ratio list, which will allow us to compare for (exact) equality.
*/
          ratio1=number_list_find_nearest(ratio1,ratio_idx_max,ratio_list1_base);
          if(IS_EQUAL(ratio0,ratio1)){
/*
We have found a maximally popular nonzero binomial. Factor it out. The factor itself will be stored to sample_list_base[sample_idx_linear]. To maintain precision, we compute the factor by averaging its value relative to both the numerator and the denominator.
*/
            ratio1=PRODUCT((sample_list_base[denominator_idx_linear]+RATIO(sample_list_base[numerator_idx_linear],ratio1)),NUMBER_MAKE(0.5f));
            SET_ZERO(sample_list_base[denominator_idx_linear]);
            SET_ZERO(sample_list_base[numerator_idx_linear]);
            substitution_count++;
          }else{
            SET_ZERO(ratio1);
          }
        }
        sample_list_base[sample_idx_linear]=ratio1;
        sample_idx_linear++;
        numerator_idx_linear++;
      }
      if(substitution_count!=population){
/*
Something went horribly wrong.
*/
        printf("\nERROR: Report this bug: (substitution_count!=population) with\nnumerator_idx=0x%08X and denominator_idx=0x%08X.\n",(u32)(numerator_idx),(u32)(denominator_idx));
        exit(1);
      }
/*
At most, this new binomial will have population_max==population.
*/
      population_list_base[sample_idx_max-1]=population; 
      sample_idx_max++;
    }while((binomial_idx1++)!=binomial_idx1_max);
/*
Copy all the new (binomial_t)s to the aggregating list at binomial_list0_base.
*/
    binomial_list0_post=&binomial_list0_base[binomial_count0];
    memcpy((void *)(binomial_list0_post),(const void *)(binomial_list1_base),binomial_idx1*sizeof(binomial_t));
    *binomial_count0_base=binomial_count0+binomial_idx1;
  }
  string_free(sort_tag_list1_base);
  string_free(sort_tag_list0_base);
  sort_free(sort_string_base);
  string_free(ratio_list2_base);
  string_free(ratio_list1_base);
  string_free(ratio_list0_base);
  return status;
}

u8
binomial_list_make(u64 *nonzero_sample_count_base,u64 *binomial_count_base,binomial_t **binomial_list_base_base,u64 *sample_idx_max_max_base,u64 *sample_idx_max_base,NUMBER **sample_list_base_base){
  u64 binomial_count0;
  u64 binomial_count0_new;
  u64 binomial_count0_max;
  u64 binomial_count1;
  u64 binomial_count1_max;
  u64 binomial_count1_max_prv;
  binomial_t *binomial_list0_base;
  binomial_t *binomial_list1_base;
  u64 nonzero_sample_count;
  u64 population;
  i64 *population_list_base;
  u8 progress_status;
  NUMBER sample;
  u64 sample_idx_linear;
  u64 sample_idx_linear_count;
  u64 sample_idx_max;
  u64 sample_idx_max_max;
  u64 sample_idx_max_new;
  u64 sample_idx_max_prv;
  NUMBER *sample_list_base;
  u8 status;

  sample_list_base=*sample_list_base_base;
  nonzero_sample_count=*nonzero_sample_count_base;
  sample_idx_max_max=*sample_idx_max_max_base;
  sample_idx_max=*sample_idx_max_base;
/*
We don't know how many (binomial_t)s in the binomial list (*binomial_list0_base) we'll actually need, which is binomial_count0_max. And likewise for *binomial_list1_base and binomial_count1_max, which is the list that gets appended to *binomial_list0_base. In any event, both counts must be >=(sample_idx_max+1). Start with double that, as a rule of thumb.
*/
  binomial_count0_max=SAMPLE_COUNT<<1;
  binomial_count1_max=binomial_count0_max;
  binomial_list0_base=(binomial_t *)(string_item_list_create(binomial_count0_max,sizeof(binomial_t)));
  status=!binomial_list0_base;
  binomial_list1_base=(binomial_t *)(string_item_list_create(binomial_count0_max,sizeof(binomial_t)));
  status|=!binomial_list1_base;
  population_list_base=(i64 *)(string_item_list_create(binomial_count0_max,U64_SIZE));
  status|=!population_list_base;
  if(!status){
    binomial_count0=0;
    sample_idx_max_prv=0;
    do{
      progress_status=0;
/*
We need sample_idx_max (binomial_t)s at binomial_list1_base and (i64)s at population_list_base, which is 1 binomial_t for each denominator and 1 i64 for each population.
*/
      if(binomial_count1_max<sample_idx_max){
/*
*binomial_list1_base is too small. Try to reallocate to double current needs.
*/
        print("Expanding populest binomial list...");
        binomial_count1_max_prv=binomial_count1_max;
        binomial_count1_max=sample_idx_max<<1;
        string_free(binomial_list1_base);
        binomial_list1_base=(binomial_t *)(string_item_list_create(binomial_count1_max,sizeof(binomial_t)));
        status=!binomial_list1_base;
        print(" resuming search...\n");
      }
      if(!status){
        status=binomial_list_get(sample_idx_max_prv,sample_idx_max-1,sample_list_base,&binomial_count1,binomial_list1_base,population_list_base,&population);
        sample_idx_max_prv=sample_idx_max;
        if(!status){
/*
If we haven't discovered any new (binomial_t)s (highly unlikely), then we're done.
*/
          if(binomial_count1){
/*
If the population of the most popular binomial is <2, then we have failed to discover a substitution that will reduce compute load. In other words, we're done.
*/
            if(population>=POPULATION_MIN){
              printf("Substituting %d binomials with population %d...\n",(u32)(binomial_count1),(u32)(population));
              fflush(stdout);
              binomial_count0_new=binomial_count0+binomial_count1;
              if(binomial_count0_new>binomial_count0_max){
/*
We don't have enough binomial space at binomial_list0_base to integrate binomial_count1 (binomial_t)s from *binomial_list1_base. Try to expand to double current needs.
*/
                print("Expanding main binomial list...");
                binomial_count0_max=binomial_count0_new<<1;
                status=string_item_list_realloc(binomial_count0_max,binomial_count0,sizeof(binomial_t),(u8 **)(&binomial_list0_base));
/*
Assume that the reallocation succeeded. If not, we'll catch the error below.
*/
                print(" resuming search...\n");
              }
              sample_idx_max_new=sample_idx_max+binomial_count1;
              if(sample_idx_max_new>sample_idx_max_max){
/*
We don't have enough sample space at sample_list_base to integrate binomial_count1 new samples corresponding to as many (binomial_t)s from *binomial_list1_base. Try to expand to approximately double current needs.
*/
                print("Expanding sample and population lists...");
                sample_idx_max_max=sample_idx_max_new<<1;
                status|=string_item_list_realloc(sample_idx_max_max+1,sample_idx_max+1,COEFF_COUNT*sizeof(NUMBER),(u8 **)(&sample_list_base));
                status|=string_item_list_realloc(sample_idx_max_max+1,sample_idx_max+1,U64_SIZE,(u8 **)(&population_list_base));
/*
Assume that the reallocation succeeded. If not, we'll catch the error below.
*/
                print(" resuming search...\n");
              }
              if(!status){
/*
Merge *binomial_list1_base into *binomial_list0_base, and make all appropriate substitutions within *sample_list_base.
*/
                status=binomial_list_merge(population-2,population_list_base,&binomial_count0,binomial_list0_base,binomial_count1-1,binomial_list1_base,sample_list_base);
/*
Assume that the merge succeeded. If not, we'll catch the error below.
*/
                sample_idx_max=sample_idx_max_new;
              }
              if(!status){
                progress_status=1;
/*
We took (binomial_count*population*2) nonzero terms out of *sample_list_base, then added (binomial_count*population) new nonzero terms due to the definition of new (binomial_t)s. Net, we subtracted (binomial_count*population) nonzero terms.
*/
                nonzero_sample_count-=binomial_count1*population;
                printf("We have %d binomials and about %d monomials.\nSearching for more substitutions...\n",(u32)(binomial_count0),(u32)(nonzero_sample_count));
                fflush(stdout);
              }
            }
          }
        }
      }
    }while(progress_status&&(!status));
    if(!status){
      sample_idx_linear_count=(sample_idx_max+1)*COEFF_COUNT;
      nonzero_sample_count=0;
      for(sample_idx_linear=0;sample_idx_linear<sample_idx_linear_count;sample_idx_linear++){
        sample=sample_list_base[sample_idx_linear];
        if(IS_NONZERO(sample)){
          nonzero_sample_count++;
        }
      }
      printf("No more substitutions.\nExactly %d nonzero monomials left.\n",(u32)(nonzero_sample_count));
      *nonzero_sample_count_base=nonzero_sample_count;
      *binomial_list_base_base=binomial_list0_base;
      *binomial_count_base=binomial_count0;
      *sample_idx_max_max_base=sample_idx_max_max;
      *sample_idx_max_base=sample_idx_max;
      *sample_list_base_base=sample_list_base;
    }
  }
  string_free(population_list_base);
  string_free(binomial_list1_base);
  return status;
}

int
main(int argc, char *argv[]){
  u64 binomial_count;
  u64 binomial_idx;
  binomial_t *binomial_list_base;
  binomial_out_t *binomial_out_list_base;
  u64 binomial_out_list_size;
  u8 buffer_base[U64_SIZE];
  u64 coeff_idx;
  u64 coeff_idx_max;
  u32 file_idx;
  u64 file_out_size;
  u64 file_size;
  u8 *filename_in_base;
  u8 *filename_ptr;
  FILE *handle_in;
  FILE *handle_out;
  orthohack_header_t header;
  u64 monomial_count;
  monomial_out_t *monomial_out_list_base;
  u64 monomial_out_list_size;
  u64 nonzero_sample_count;
  u64 number_in_idx;
  number_in_unpadded_t *number_in_list_base;
  u64 pathname_size;
  NUMBER sample;
  u64 sample_idx;
  u64 sample_idx_max;
  u64 sample_idx_max_max;
  NUMBER_IN sample_in;
  u64 sample_linear_idx;
  NUMBER *sample_list_base;
  NUMBER_OUT sample_out;
  u64 signature;
  u8 status;
  u8 warning_status;

  print_intro("Preprocessor");
#if (PRECISION_IN==2)||(PRECISION_INTERNAL==2)||(PRECISION_OUT==2)
  if(sizeof(long double)!=sizeof(u96_t)){
    print_error_exit("80-bit precision is not available. Change PRECISION_IN, PRECISION_INTERNAL, and PRECISION_OUT accordingly.");
  }
#endif
  if(((NUMBER_IN_U16_PAD_LO_COUNT+((NUMBER_COMPONENT_IDX_MAX+1)*(NUMBER_IN_U16_IDX_MAX+1))+NUMBER_IN_U16_PAD_HI_COUNT)<<U16_SIZE_LOG2)!=sizeof(NUMBER_IN)){
    print_error_exit("NUMBER_IN_U16_PAD_LO_COUNT, NUMBER_IN_U16_PAD_HI_COUNT, and\nNUMBER_IN_U16_IDX_MAX are inconsistent with sizeof(NUMBER_IN)");
  }
  if(((NUMBER_OUT_U16_PAD_LO_COUNT+((NUMBER_COMPONENT_IDX_MAX+1)*(NUMBER_OUT_U16_IDX_MAX+1))+NUMBER_OUT_U16_PAD_HI_COUNT)<<U16_SIZE_LOG2)!=sizeof(NUMBER_OUT)){
    print_error_exit("NUMBER_OUT_U16_PAD_LO_COUNT, NUMBER_OUT_U16_PAD_HI_COUNT, and\nNUMBER_OUT_U16_IDX_MAX are inconsistent with sizeof(NUMBER_OUT)");
  }
  print_precision(PRECISION_IN,0);
  print_precision(PRECISION_INTERNAL,1);
  print_precision(PRECISION_OUT,2);
  print("Reading orthobasis vector files...\n");
/*
Allocate the sample list. As a rule of thumb, start out with about twice as much space as we need at the moment.
*/
  sample_idx_max=SAMPLE_COUNT-1;
  sample_idx_max_max=sample_idx_max<<1;
  sample_list_base=(NUMBER *)(string_item_list_create(sample_idx_max_max+1,COEFF_COUNT*sizeof(NUMBER)));
  pathname_size=strlen(pathname_base);
/*
Set filename_in_base to the base of a filename containing: (the pathname at pathname_base)+(9 decimal digits for the orthobasis number)+".bin"+NULL.
*/
  filename_in_base=string_create(pathname_size+9+1+3+1);
  number_in_list_base=(number_in_unpadded_t *)(string_create(SAMPLE_COUNT*sizeof(number_in_unpadded_t)));
  if(!(sample_list_base&&filename_in_base&&number_in_list_base)){
    print_error_exit_out_of_memory();
  }
  handle_out=file_open(filename_base,FILE_APPEND);
  if(!(handle_out&&((file_size_get_rewind(handle_out,&file_out_size))==0)&&(file_out_size==0))){
    print_error_exit("Output file already exists");
  }
/*
Point filename_in_base to first digit of the (9 decimal digits for the orthobasis number) number located at the end of *filename_in_base.
*/
  memcpy(filename_in_base,(const char *)(pathname_base),pathname_size);
  filename_ptr=filename_in_base+pathname_size;
/*
Append "bin"+NULL.
*/
  filename_ptr[0xA]='b';
  filename_ptr[0xB]='i';
  filename_ptr[0xC]='n';
  filename_ptr[0xD]='\x00';
  warning_status=0;
  file_idx=0;
  sample_idx=0;
  do{
    snprintf(filename_ptr,9+1,"%09d",file_idx);
/*
Replace the "." that sprintf just overwrote.
*/
    filename_ptr[9]='.';
    handle_in=file_open(filename_in_base,FILE_READ);
    if(!handle_in){
      print_error_string_exit("Cannot read ",filename_in_base);
    }
    status=file_read(SAMPLE_COUNT*sizeof(number_in_unpadded_t),(u8 *)(number_in_list_base),handle_in);
    if(status){
      printf("%s:\n",filename_in_base);
      print_error_value_exit("Could not read expected size of %d bytes",SAMPLE_COUNT*sizeof(number_in_unpadded_t));
    }
    if(!warning_status){
/*
See if we can read an extra byte, which means that the file is larger than expected and thus probably not the data that we think we're reading.
*/
      status=file_read(1,buffer_base,handle_in);
      if(!status){
        printf("%s:\n",filename_in_base);
        printf("WARNING: File size exceeds expected %d bytes (wrong data format?)",(u32)(SAMPLE_COUNT*sizeof(number_in_unpadded_t)));
/*
Make an explicit print() call in order to do fflush(stdout).
*/
        print(".\n");
/*
We don't need to print this warning for every single file.
*/
        warning_status=1;
      }
    }
    file_close(&handle_in);
/*
Convert (NUMBER_IN)s from the source file to (NUMBER)s, transposing them into a matrix COEFF_COUNT wide by SAMPLE_COUNT high (with enough space for SAMPLE_COUNT_MAX total samples). (Thus successive memory addresses correspond first to successive coefficients, and then to successive sample indexes.)
*/
    for(number_in_idx=0;number_in_idx<SAMPLE_COUNT;number_in_idx++){
      NUMBER_IN_PAD(&number_in_list_base[number_in_idx],sample_in);      
      sample_list_base[sample_idx]=NUMBER_MAKE(sample_in);
      sample_idx+=COEFF_COUNT;
    }
    sample_idx-=(SAMPLE_COUNT*COEFF_COUNT)-1;
    file_idx++;
  }while(file_idx!=COEFF_COUNT);
  string_free(number_in_list_base);
  string_free(filename_in_base);
  print("Flushing samples with magnitudes up to PRECISION_ZERO_LIMIT, to 0...\n");
  nonzero_sample_count=number_list_flush_zero((SAMPLE_COUNT*COEFF_COUNT)-1,sample_list_base);
  printf("%d of %d terms are nonzero. Building binomial tree...\n",(u32)(nonzero_sample_count),(u32)(COEFF_COUNT*SAMPLE_COUNT));
  fflush(stdout);
  status=binomial_list_make(&nonzero_sample_count,&binomial_count,&binomial_list_base,&sample_idx_max_max,&sample_idx_max,&sample_list_base);
  if(!binomial_count){
    print_error_exit("No binomials found. Orthohack cannot help you.");
  }
  binomial_out_list_base=(binomial_out_t *)(string_item_list_create(binomial_count,sizeof(binomial_out_t)));
  status|=!binomial_out_list_base;
  monomial_out_list_base=(monomial_out_t *)(string_item_list_create(sample_idx_max+1,sizeof(monomial_out_t)));
  status|=!monomial_out_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  print("Writing output file...\n");
  string_zero(sizeof(orthohack_header_t),(u8 *)(&header));
  header.signature=ORTHOHACK_FILE_SIGNATURE;
  header.size_following=(sizeof(orthohack_header_t)-(U64_SIZE<<1))+(COEFF_COUNT*U64_SIZE)+(nonzero_sample_count*sizeof(monomial_out_t))+(binomial_count*sizeof(binomial_out_t));
  header.coeff_idx_max=COEFF_COUNT-1;
  header.sample_idx_max=SAMPLE_COUNT-1;
  header.binomial_idx_max=binomial_count-1;
  header.precision_zero_limit=PRECISION_ZERO_LIMIT;
  header.precision_ratio_limit_minus_1=PRECISION_RATIO_LIMIT-1;
  header.number_component_idx_max=NUMBER_COMPONENT_IDX_MAX;
  header.number_u16_idx_max=NUMBER_OUT_U16_IDX_MAX;
  status=file_write(sizeof(orthohack_header_t),(u8 *)(&header),handle_out);
  if(status){
    print_error_string_exit("Cannot write to ",filename_in_base);
  }
  header.size_following-=(sizeof(orthohack_header_t)-(U64_SIZE<<1));
  coeff_idx=0;
  do{
    monomial_count=0;
    sample_idx=0;
    sample_linear_idx=coeff_idx;
    do{
      sample=sample_list_base[sample_linear_idx];
      sample_linear_idx+=COEFF_COUNT;
      if(IS_NONZERO(sample)){
        sample_out=NUMBER_OUT_MAKE(sample);
        NUMBER_OUT_UNPAD(sample_out,&monomial_out_list_base[monomial_count].ratio);
        monomial_out_list_base[monomial_count].sample_idx=sample_idx;
        monomial_count++;
      }
    }while((sample_idx++)!=sample_idx_max);
    header.size_following-=U64_SIZE;
    status=file_write(U64_SIZE,(u8 *)(&monomial_count),handle_out);
    if(monomial_count){
      monomial_out_list_size=monomial_count*sizeof(monomial_out_t);
      header.size_following-=monomial_out_list_size;
      status|=file_write(monomial_out_list_size,(u8 *)(monomial_out_list_base),handle_out);
    }
    if(status){
      print_error_string_exit("Cannot write to ",filename_in_base);
    }
  }while((coeff_idx++)!=(COEFF_COUNT-1));
  for(binomial_idx=0;binomial_idx<binomial_count;binomial_idx++){
    binomial_out_list_base[binomial_idx].denominator_idx=binomial_list_base[binomial_idx].denominator_idx;
    binomial_out_list_base[binomial_idx].numerator_idx=binomial_list_base[binomial_idx].numerator_idx;
    sample=binomial_list_base[binomial_idx].ratio;
    sample_out=NUMBER_OUT_MAKE(sample);
    NUMBER_OUT_UNPAD(sample_out,&binomial_out_list_base[binomial_idx].ratio);
  }
  binomial_out_list_size=binomial_count*sizeof(binomial_out_t);
  header.size_following-=binomial_out_list_size;
  status=file_write(binomial_out_list_size,(u8 *)(binomial_out_list_base),handle_out);
  status|=file_close(&handle_out);
  if(status){
    print_error_string_exit("Cannot write to ",filename_in_base);
  }
  if(header.size_following){
    print_error_exit("File is corrupt because header.size_following is wrong.\n");
  }
  status=file_close(&handle_out);
  if(!status){
    printf("Done.\n");
  }else{
    print_error_exit("Cound not close output file");
  }
  return 0;
}

