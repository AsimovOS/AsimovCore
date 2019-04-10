module T
	import MsgPack
	function dispatch_event(topic::String, args::Vector{UInt8})
		a = MsgPack.unpack(Vector{UInt8}(args))
		#s = IOBuffer(args)
		#b = read(s, UInt8) #Type (array)
		#a = read(s, UInt8) #Type of first element
		#c = read(s, UInt8)
		println("Dispatching on topic $topic with args $a")
	end

end
