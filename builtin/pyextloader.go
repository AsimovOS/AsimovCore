package builtin
/*
#cgo pkg-config: python-3.7
#include <stdio.h>
extern void loadPyExt(char* filename);
extern void initPy();
extern void dispatchEventPy(char* topic, void* params, size_t size);
*/
import "C"
import (
	"github.com/vmihailenco/msgpack"
	"unsafe"
	"fmt"
//	"time"
)

func LoadPyExt(filename string) {
	C.loadPyExt(C.CString(filename))
}

func InitPyExtLoader() {
	C.initPy()
}

//export DispatchGoFromPy
func DispatchGoFromPy(t *C.char, params *C.uint, length C.size_t) {
	fmt.Printf("Running DispatchGoFromPy with topic %s", C.GoString(t))
	DispatchGo(t, params, length)
	
	ev := <-PyChan
	println("Calling DispatchPy from DispatchGoFromPy")
	DispatchPy(ev.Topic, ev.Params...)
	println("End of call to DispatchGoFromPy")
}


func DispatchPy(topic string, arg ...interface{}) {
	b, err := msgpack.Marshal(arg)
        if err != nil {
                panic(err)
        }
	//Passing pointer and length instead of using C.CString because a byte of zero anywhere in the data would cause the string to be truncated
	C.dispatchEventPy(C.CString(topic), unsafe.Pointer(&b[0]), C.ulong(len(b)))
	println("End of call to DispatchPy")
}
