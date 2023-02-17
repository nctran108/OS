/*
 * The average procedure receives an array of real
 * numbers and returns the average of their
 * values. This toy service handles a maximum of
 * 100 numbers.
 * http://www.linuxjournal.com/article/2204?page=0,1
 */

const MAXAVGSIZE  = 100;

struct input_data 
  {
  double input_data<100>;
  };


typedef struct input_data input_data;

program SORTPROG {
    version SORTERS {
        input_data SORT(input_data) = 1;
    } = 1;
} = 4278;
