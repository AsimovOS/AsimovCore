package builtin

/*
extern void loadCExt(char*);
*/
import "C"
import (
	"os"
	"unsafe"
	"reflect"
	"github.com/vmihailenco/msgpack"
)

type AsiEvent struct {
	Topic string
	Params []interface{}
}
var PyChan chan AsiEvent = make(chan AsiEvent, 5)
var JLChan chan AsiEvent = make(chan AsiEvent, 5)

//export ShutdownSys
func ShutdownSys() {
	os.Exit(0)
}

func LoadCExt(filename string) {
	C.loadCExt(C.CString(filename))
}
//export DispatchGo
func DispatchGo(t *C.char, params *C.uint, length C.size_t) {
	/*MAIN USE IS FOR C EXTENSIONS AND AS A HELPER GO FUNCTION FOR OTHER LANGUAGES*/
	//slice := (*[1 << 28]C.uint)(unsafe.Pointer(params))[:length:length]
	var slice []byte
	l := int(length)
	sliceHeader := (*reflect.SliceHeader)((unsafe.Pointer(&slice)))
	sliceHeader.Cap = l
 	sliceHeader.Len = l
 	sliceHeader.Data = uintptr(unsafe.Pointer(params))
	var p []interface{}
	_ = msgpack.Unmarshal([]byte(slice), &p)
	PyChan <- AsiEvent{Topic: C.GoString(t), Params: p}
	JLChan <- AsiEvent{Topic: C.GoString(t), Params: p}
	//DispatchJL(C.GoString(topic), p...)
}
