#include <stdio.h>

#include "libs.h"

int global_value1 = 78;

void dump(void)
{
  printf("&global_value1: %p, global_value1: %d\n",
         &global_value1, global_value1);
  printf("&global_value2: %p, global_value2: %d\n",
         &global_value2, global_value2);

  global_value1 *= 10;
  global_value2 *= 10;
}
