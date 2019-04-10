package builtin

/*
#cgo darwin CFLAGS: -I/Applications/Julia-1.0.app/Contents/Resources/julia/include/ -I/Applications/Julia-1.0.app/Contents/Resources/julia/include/julia -g3
#cgo darwin LDFLAGS: -ljulia -L/Applications/Julia-1.0.app/Contents/Resources/julia/lib/ -L/Applications/Julia-1.0.app/Contents/Resources/julia/lib/julia/
#include <stdio.h>
extern void loadJLExt(char* filename);
extern void dispatchEventJL(char* topic, void* params, size_t size);
extern void jlInit();
*/
import "C"
import (
	"fmt"
	"unsafe"
	"github.com/vmihailenco/msgpack"
)
func Init() {
	C.jlInit()
}

func LoadJLExt(filename string) {
	C.loadJLExt(C.CString(filename))
}

func DispatchJL(topic string, arg ...interface{}) {
	b, err := msgpack.Marshal(arg)
	if err != nil {
    		panic(err)
	}
	C.dispatchEventJL(C.CString(topic), unsafe.Pointer(&b[0]), C.ulong(len(b)))
}

//export DispatchGoFromJL
func DispatchGoFromJL(t *C.char, params *C.uint, length C.size_t) {
        fmt.Printf("Running DispatchGoFromJL with topic %s", C.GoString(t))
        DispatchGo(t, params, length)
        ev := <-JLChan
        DispatchJL(ev.Topic, ev.Params...)
}
