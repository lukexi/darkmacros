#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "realtalk.h"

#define ArrayLength(a) (sizeof(a)/sizeof(*a))

/*
Create dummy NanoVG functions that just print their
arguments to test our API generator.
*/

void nvgBeginPath(void* ctx) {
    printf("nvgBeginPath %p\n", ctx);
}

void nvgMoveTo(void* ctx, float a, float b) {
    printf("nvgMoveTo %p %f %f\n", ctx, a, b);
}

void nvgBezierTo(void* ctx, float a, float b, float c, float d) {
    printf("nvgMoveTo %p %f %f %f %f\n", ctx, a, b, c, d);
}

void nvgSetFont(void* ctx, const char* a, float b) {
    printf("nvgMoveTo %p %s %f\n", ctx, a, b);
}


// Partially inspired by Rygorous's simple GL function loader
// https://gist.github.com/rygorous/16796a0c876cf8a5f542caddb55bce8a

/*
First we define a list of macros representing NanoVG functions.
We're going to interpret this list in 2 different ways
by changing the definition of the NVGFUNC macro
before evaluating NVGFUNCTIONS multiple times.

The first argument to NVGFUNC is the name of the NanoVG function,
and the remaining arguments describe the types of the arguments
along with their positional index.
(1-based, as the 0th arg is the function name)
*/

#define NVGFUNCTIONS \
    NVGFUNC(nvgBeginPath) \
    NVGFUNC(nvgMoveTo, FLOAT(1), FLOAT(2)) \
    NVGFUNC(nvgBezierTo, FLOAT(1), FLOAT(2), FLOAT(3), FLOAT(4)) \
    NVGFUNC(nvgSetFont, STRING(1), FLOAT(2)) \

/*
In the first interpretation, we define NVGFUNC
to define a wrapped version of the given NanoVG function name.
These all have the form
void nvgOriginal_rt(rt_state* rt, rt_ref args, NVGContext* nvg) {}
where 'args' is an RT array of the arguments to the function.

(##__VA_ARGS__ means delete the preceding comma if the varargs are empty)
*/

#define NVGFUNC(name, ...) \
    void name##_rt(rt_state* rt, rt_ref args, void* nvg) { \
        name(nvg, ##__VA_ARGS__); }

/*
Here we define how to expand the arguments to NVGFUNC.
We expand them to calls to extract the proper value
of the given type, from the given index
in the 'args' RT array.
*/

#define FLOAT(i) rt_ref_to_number(rt, rt_get_value_at_index(rt, args, i))
#define STRING(i) rt_ref_to_string(rt, rt_get_value_at_index(rt, args, i))

/* We now evaluate NVGFUNCTIONS the first time,
which will use the just-defined macros to expand the
list of
NVGFUNC(nvgMoveTo, FLOAT(1), FLOAT(2))
into wrapped definitions for those functions.
*/
NVGFUNCTIONS

/*
The final output will be a bunch of functions that look like:
void nvgSetFont_rt(rt_state* rt, rt_ref args, void* nvg) {
    nvgSetFont(nvg,
        rt_ref_to_number(rt, rt_get_value_at_index(rt, args, 1)),
        rt_ref_to_string(rt, rt_get_value_at_index(rt, args, 2))
        );
}

Great! That's step one.

Having done this, we undefine NVGFUNC so we can give
it a new meaning below.
*/
#undef NVGFUNC

/* Now we would like to generate a list of the wrapped functions
keyed by their original names. We create a struct to represent this:
*/

typedef void (*nvg_func_rt)(rt_state* rt, rt_ref args, void* nvg);
typedef struct {
    char*       Name;
    nvg_func_rt Function;
} nvg_func_entry;

/* We redefine NVGFUNC to expand to an entry in the function list,
and evaluate NVGFUNCTIONS a second time with this new definition to
generate the list of wrapped functions.
*/
#define NVGFUNC(name, ...) { #name, &name##_rt },
nvg_func_entry NVGFunctions[] = {
    NVGFUNCTIONS
};
#undef NVGFUNC

/*
This will give us something like:
nvg_func_entry NVGFunctions[] = {
    { "nvgBeginPath", &nvgBeginPath_rt },
    { "nvgMoveTo", &nvgMoveTo_rt },
    { "nvgBezierTo", &nvgBezierTo_rt },
    { "nvgSetFont", &nvgSetFont_rt },
};

And we're done!

*/

void CallRTNVGFunc(rt_state* rt, void* NVG, rt_ref FuncRef) {

    const char* FuncName = rt_ref_to_string(rt,
        rt_get_value_at_index(rt, FuncRef, 0));

    for (int i = 0; i < ArrayLength(NVGFunctions); i++)
    {
        nvg_func_entry *Entry = &NVGFunctions[i];
        if (strcmp(Entry->Name, FuncName) == 0)
        {
            (*Entry->Function)(rt, FuncRef, NVG);
        }
    }
}

int main(int argc, char const *argv[])
{
    rt_state* rt = rt_create_state(NULL);
    void* NVG = &"pretend nanovg context pointer";

    rt_ref FuncArgs[] = {
        rt_ref_from_cstring(rt, "nvgMoveTo"),
        rt_ref_from_number(rt, 123),
        rt_ref_from_number(rt, 456)
    };
    rt_ref FuncRef = rt_ref_for_array(rt, FuncArgs, ArrayLength(FuncArgs));

    CallRTNVGFunc(rt, NVG, FuncRef);

    // Calls
    // nvgMoveTo_rt(rt, array_ref, NVG);
    // which in turn calls nvgMoveTo.

    return 0;
}

