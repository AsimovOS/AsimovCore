#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

void loadCExt(char* filename) {
   void *lib_handle;
   int (*init)(void);
   void (*ext_main)(void);
   int x = 0;
   char *error;
   lib_handle = dlopen(filename, RTLD_LAZY);
   if (!lib_handle)
   {
      fprintf(stderr, "%s\n", dlerror());
      return;
   }
   init = dlsym(lib_handle, "init_jl_runtime");
   ext_main = dlsym(lib_handle, "ext_main");
   if ((error = dlerror()) != NULL) 
   {
      fprintf(stderr, "%s\n", error);
      return;
   }
   (*init)();
   //(*ext_main)();
   dlclose(lib_handle);
}

