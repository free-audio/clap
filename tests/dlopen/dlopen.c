#include <stdio.h>
#include <dlfcn.h>

#ifndef RLTD_DEEPBIND
# define RTLD_DEEPBIND 0
#endif

int main(int argc, char **argv)
{
  void *handle1 = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
  if (!handle1) {
    printf("%s\n", dlerror());
    return 1;
  }

  void *sym1 = dlsym(handle1, "dump");
  ((void (*)(void))sym1)();

  void *handle2 = dlopen(argv[2], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
  if (!handle2)
    return 1;

  void *sym2 = dlsym(handle2, "dump");
  ((void (*)(void))sym2)();

  printf("syms: %p, %p\n", sym1, sym2);

  dlclose(handle1);
  dlclose(handle2);

  handle1 = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
  if (!handle1) {
    printf("%s\n", dlerror());
    return 1;
  }

  sym1 = dlsym(handle1, "dump");
  ((void (*)(void))sym1)();
  dlclose(handle1);

  handle2 = dlopen(argv[2], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
  if (!handle2)
    return 1;

  sym2 = dlsym(handle2, "dump");
  ((void (*)(void))sym2)();
  dlclose(handle1);

  printf("syms: %p, %p\n", sym1, sym2);
  return 0;
}
