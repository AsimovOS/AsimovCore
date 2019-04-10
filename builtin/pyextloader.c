#include <Python.h>
#include <stdio.h>

PyObject *pName, *pModule, *pFunc;
extern void DispatchGo(char* topic, unsigned int* params, size_t length);
extern void DispatchGoFromPy(char* topic, unsigned int* params, size_t length);

void loadPyExt(char* filename) {
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	PyRun_SimpleString(
	"import sys\n"
	"sys.path.append('.')\n"
);
	pName = PyUnicode_DecodeFSDefault("main");
	printf("Importing module\n");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (pModule != NULL) {
		printf("Getting pFunc\n");
	        pFunc = PyObject_GetAttrString(pModule, "dispatch_ev_py");
        	/* pFunc is a new reference */
	}
	printf("%p\n", pFunc);
	/* Release the thread. No Python API allowed beyond this point. */
	PyGILState_Release(gstate);
}

void dispatchEventPy(char* topic, void* params, size_t size) {
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	printf("%p\n", pFunc);
	char* p = (char*) params;
	PyObject *pArgs, *pValue;
	if (pFunc) {
            pArgs = PyTuple_New(size);
            for (size_t i = 0; i < size; ++i) {
                pValue = PyLong_FromSize_t(p[i]);
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return;
                }
                //pValue reference stolen here:
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pArgs = PyBytes_FromStringAndSize(p, PyLong_AsSsize_t(PyLong_FromSize_t(size)));
            PyObject* dispatchArgs = PyTuple_New(2);
            PyTuple_SetItem(dispatchArgs, 0, PyUnicode_FromString(topic));
            PyTuple_SetItem(dispatchArgs, 1, pArgs);
            pValue = PyObject_CallObject(pFunc, dispatchArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find dispatch function\n");
        }
	/* Release the thread. No Python API allowed beyond this point. */
	PyGILState_Release(gstate);
	if (PyGILState_Check()) {
    		printf("\nERROR: GIL HELD LONGER THAN EXPECTED\n");
	}
	printf("\nEnd of call to dispatchEventPy\n");
}

/*BEGIN PYTHON MODULE*/
static PyObject*
emb_dispatch(PyObject *self, PyObject *args)
{
    printf("\n\tCALLING EMB_DISPATCH\n");
    const char* topic;
    Py_buffer  params;
    size_t size;
    if(!PyArg_ParseTuple(args, "sy*", &topic, &params)) {
       printf("PARSING TUPLE FAILED");
       return NULL;
    }
    unsigned int* p = (unsigned int*) params.buf;
    size = (size_t) params.len;
    DispatchGoFromPy((char*) topic, p, size);
    if (PyErr_Occurred())
                PyErr_Print();    
    return PyLong_FromLong(5);
}

static PyMethodDef EmbMethods[] = {
    {"dispatch", emb_dispatch, METH_VARARGS,
     "Dispatch event to Go"},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_emb(void)
{
    return PyModule_Create(&EmbModule);
}


/*INIT FUNCTION*/
void initPy() {
        PyImport_AppendInittab("emb", PyInit_emb);
        Py_Initialize();
}
