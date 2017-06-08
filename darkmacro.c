#include <stdio.h>
#include <stdarg.h>
#include "realtalk.h"
#define ArrayLength(a) (sizeof(a)/sizeof(*a))

void nvgBeginPath(void* ctx) {
    printf("nvgBeginPath %p\n", ctx);
}

void nvgMoveTo(void* ctx, float a, float b) {
    printf("nvgMoveTo %p %f %f\n", ctx, a, b);
}

void nvgBezierTo(void* ctx, float a, float b, float c, float d) {
    printf("nvgMoveTo %p %f %f %f %f\n", ctx, a, b, c, d);
}

void nvgSetFont(void* ctx, char* a, float b) {
    printf("nvgMoveTo %p %s %f\n", ctx, a, b);
}

char* getString(rt_state* rt) {
    return "foo";
}
float getFloat(rt_state* rt) {
    return 123.4;
}

/*
void nvgBeginPath_rt(rt_state* rt, void* ctx) {
    nvgBeginPath(ctx
        );
}

void nvgMoveTo_rt(rt_state* rt, void* ctx) {
    nvgMoveTo(ctx
        , getFloat(rt)
        , getFloat(rt)
        );
}
*/


#define BEGIN(name) \
void name##_rt(rt_state* rt, rt_ref args, void* nvg) { \
    name(nvg
#define FLOAT(i) , rt_ref_to_number(rt, rt_get_value_at_index(rt, args, i))
#define STRING(i) , rt_ref_to_string(rt, rt_get_value_at_index(rt, args, i))
#define END ); };
BEGIN(nvgMoveTo) FLOAT(0) FLOAT(1) END

int main(int argc, char const *argv[])
{
    rt_state* rt = rt_create_state(NULL);

    rt_ref fields[] = {
        // rt_ref_from_cstring(rt, "you found me!"),
        rt_ref_from_number(rt, 123),
        rt_ref_from_number(rt, 456)
    };
    rt_ref array_ref = rt_ref_for_array(rt, fields, ArrayLength(fields));

    printf("Hi\n");
    nvgMoveTo_rt(rt, array_ref, NULL);

    return 0;
}





/*
typedef enum {
    ArgEnd,
    ArgFloat,
    ArgString,
} nvg_arg_type;

typedef struct {
    char* FunctionName;
    void* Function;
    nvg_arg_type ArgumentType[10];
} nvg_function_pointer;

#define F(name) #name, name
const nvg_function_pointer NVGFunctions[] = {
    { F(nvgBeginPath), {ArgEnd} },
    { F(nvgMoveTo), {ArgFloat, ArgFloat, ArgEnd} },
    { F(nvgBezierTo), {ArgFloat, ArgFloat, ArgFloat, ArgFloat, ArgEnd} },
};

*/
