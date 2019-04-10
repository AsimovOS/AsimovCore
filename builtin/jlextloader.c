#define JULIA_ENABLE_THREADING
#include "julia.h"
#include <stdio.h>
JULIA_DEFINE_FAST_TLS()


jl_module_t* mod;
jl_function_t* dispatch;
jl_function_t* ini;

void jlInit() {
	jl_init();
}

void loadJLExt(char* filename) {
        mod = (jl_module_t*) jl_load(jl_main_module, filename);
        dispatch = (jl_function_t*) jl_get_function(mod, "dispatch_event");
	ini = (jl_function_t*) jl_get_function(mod, "init");
	jl_call0(ini);
	if (jl_exception_occurred()) {
	    printf("%s \n", jl_typeof_str(jl_exception_occurred()));
            jl_eval_string("bt = catch_backtrace()");
  	    jl_eval_string("println(bt)");
	}
}

void dispatchEventJL(char* topic, void* params, size_t size) {
	uint* p = (uint*) params;
	jl_array_t* arr = NULL;
	jl_value_t* array_type = NULL;
	JL_GC_PUSH2(&arr, &array_type);
	array_type = jl_apply_array_type((jl_value_t*)jl_uint8_type, 1);
	arr = jl_alloc_array_1d(array_type, size);
	uint* arrData = (uint*) jl_array_data(arr);
	for(size_t i=0; i<jl_array_len(arr); i++)
    		arrData[i] = p[i];
        jl_call2(dispatch, jl_cstr_to_string(topic), (jl_value_t*)arr);
	if (jl_exception_occurred())
	    printf("%s \n", jl_typeof_str(jl_exception_occurred()));
	JL_GC_POP();
}

