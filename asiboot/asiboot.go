package main

import (
	"github.com/AsimovOS/AsimovCore/builtin"
	"sync"
	"runtime"
//	"fmt"
)

var wg sync.WaitGroup

func main() {
	wg.Add(1)
	//go LoadJLExtensions()
	go LoadPyExtensions()
	wg.Wait()
	
}

func LoadPyExtensions() {
	runtime.LockOSThread()
	builtin.InitPyExtLoader()
	builtin.LoadPyExt("main.py")
	builtin.DispatchPy("/asimov/boot", nil, 5, 7)
	builtin.DispatchPy("/asimov/boot/finished")
	/*go func() {
	for true {
	//	select {
	//	case ev := <-builtin.PyChan:
	//		builtin.DispatchPy(ev.Topic, ev.Params...)
		//default:
			//Do nothing
	//	}
	//}
	}()*/
//	wg.Done()
}

func LoadJLExtensions() {
	runtime.LockOSThread()
	wg.Add(1)
	builtin.Init()
	builtin.LoadJLExt("main.jl")
	builtin.DispatchJL("/asimov/boot", nil, 5, 7)
	for true {
                ev := <-builtin.JLChan
                builtin.DispatchJL(ev.Topic, ev.Params...)
        }
	wg.Done()
}
