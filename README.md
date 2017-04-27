## CHORD Ring Implementation
### Yue Lyu

### Instruction
Just `make` and run the binary with folloing command:<br />

For a CHORD Node:
`./host --port port --node-id id [--entry-point addr]`<br />

`--port port`               : port number    
`--node-id id`              : unique node id between [0, 8)  
`--entry-point addr`        : a number-and-dot format IP address indicating entry point, if not provided, this node is the first node in the ring

For a client:
`./client leave --port port --addr addr`<br />

`--port port`               : port number  
`--addr addr`               : a number-and-dot format IP address, indicating the leaving node