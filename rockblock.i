%module rockblock_lua

%{
// This block is for the C compiler.
// It must include ALL headers necessary for the C code in rockblock_lua_wrap.cxx
// to compile successfully. This includes headers for functions called by rockblock_9704.h
// even if those functions are not directly exposed to Lua.
#include "rockblock_9704.h"
#include "crossplatform.h" // Assuming rockblock_9704.c might call functions from here
#include "gpio.h"
#include "imt_queue.h"
#include "jspr.h"
#include "jspr_command.h"
#include "serial.h"
%}

// Tell SWIG to ignore static declarations
// This is because it's a static (internal) function and not part of the public API that should be exposed to Lua.
%ignore calculateCrc;
%ignore appendCrc;
%ignore encodeData;
%ignore decodeData;
%ignore setApi;
%ignore setSim;
%ignore setState;
%ignore checkProvisioning;
%ignore sendMoFromQueue;
%ignore listenForMt;
%ignore getHwInfo;
%ignore getSimStatus;

// This block is for SWIG's parser.
// ONLY include headers here for functions/types you want DIRECTLY callable/usable from Lua.
%include "rockblock_9704.h"

// If any of the functions/structs in rockblock_9704.h internally use types
// that are *defined* in other headers, and SWIG needs to understand those types
// for correct wrapping (e.g., if a function in rockblock_9704.h returns `serial_port_handle_t`
// and you want Lua to see `serial_port_handle_t` as a type), then you might need
// to selectively include those specific definitions or headers here, or use `%import`.
// However, if they are opaque pointers or simple data types, SWIG often handles it.