%module rockblock_lua

//typemap to force 'int8_t' values returned by C functions to be pushed as Lua numbers directly
%typemap(out) int8_t {
    lua_pushnumber(L, $1);
    SWIG_arg++; // this is to increment the lua values pushed else it doesn't know its done a thing
}

// typemap to handle 'uint16_t' when used as an input parameter.
%typemap(in) uint16_t {
    // Check if the Lua input is a number
    if (!lua_isnumber(L, $input)) {
        SWIG_fail_arg("$name", $argnum, "number"); // Error if not a number
    }
    // Convert the Lua number to uint16_t and assign it to the C argument ($1)
    $1 = (uint16_t)lua_tonumber(L, $input);
}

// typemap to auto create a char** rather than supply one from lua as an argument
%typemap(in, numinputs=0) char **RECEIVE_BUFFER (char *temp_buffer = NULL) {
    $1 = &temp_buffer; // $1 is the char** parameter in the C function signature
}

// typemap to push out a string if the above buffer isnt empty. this is a second return value in the fucntion
// e.g. local message_length, message_content = rockblock.rbReceiveMessage() 
%typemap(argout) char **RECEIVE_BUFFER {
    if (*$1 && result > 0) {
        lua_pushlstring(L, (const char*)*$1, (size_t)result); // Push the string to Lua
        SWIG_arg++; // Increment the count of Lua return values
    }
}

// apply the type maps to relevant functions. must use the correct names of variables
%apply (char **RECEIVE_BUFFER) { char **buffer };

// This block is for the C compiler, all headers for the C code in rockblock_lua_wrap.cxx to compile successfully
%{
#include "rockblock_9704.h"
#include "crossplatform.h" // Assuming rockblock_9704.c might call functions from here
#include "gpio.h"
#include "imt_queue.h"
#include "jspr.h"
#include "jspr_command.h"
#include "serial.h"
%}

// Tell SWIG to ignore static declarations as they are not part of the public API that should be exposed to Lua
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
