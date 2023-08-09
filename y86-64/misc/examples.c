/* 
 * Architecture Lab: Part A 
 * 
 * High level specs for the functions that the students will rewrite
 * in Y86-64 assembly language
 */

/* $begin examples */
/* linked list element */
typedef struct ELE {
    long val;
    struct ELE *next;
} *list_ptr;

/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
	val += ls->val;
	ls = ls->next;
    }
    return val;
}

/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls)
{
    if (!ls)
	return 0;
    else {
	long val = ls->val;
	long rest = rsum_list(ls->next);
	return val + rest;
    }
}

/* copy_block - Copy src to dest and return xor checksum of src */
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
	long val = *src++;
	*dest++ = val;
	result ^= val;
	len--;
    }
    return result;
}

/* find_min_index - Finds minimum element in specified array
 * of length len and returns its index.  Returns -1 if array 
 * or len are invalid. */
long find_min_index(long *arr, long len)
{
  long minIndex = 0;

  if(arr != 0){
    long i = 0;
    for(int i = 0; i < len; i++){
      if(arr[minIndex] > arr[i]){
	minIndex = i;
      }
    }
  }
  return minIndex;
}

/* move_min_to_front - Finds minimum element in specified array 
 * of length len and moves minimum to index 0 of array.  */
void move_min_to_front(long * arr, long len)
{
  long minIndex = find_min_index(arr, len);

  if(minIndex > 1){
    long tmp = arr[0];
    arr[0] = arr[minIndex];
    arr[minIndex] = tmp;
  }
}

/* $end examples */
