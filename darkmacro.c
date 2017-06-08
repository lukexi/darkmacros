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

char* getString(void* rt) {
    return "foo";
}
float getFloat(void* rt) {
    return 123.4;
}

/*
void nvgBeginPath_rt(void* rt, void* ctx) {
    nvgBeginPath(ctx
        );
}

void nvgMoveTo_rt(void* rt, void* ctx) {
    nvgMoveTo(ctx
        , getFloat(rt)
        , getFloat(rt)
        );
}
*/


#define BEGIN(name) \
void name##_rt(void* rt, void* ctx) { \
    int i = 0; \
    name(ctx
#define FLOAT , getFloat(rt)
#define STRING , getString(rt)
#define END ); printf("final result %d\n", i); };
BEGIN(nvgMoveTo) FLOAT FLOAT END

int main(int argc, char const *argv[])
{
    rt_state* rt = rt_create_state(NULL);

    rt_ref ref1 = rt_ref_from_cstring(rt, "foo");

    printf("Hi\n");
    nvgMoveTo_rt(NULL, NULL);

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
