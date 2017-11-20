#pragma once
#include "python_api.h"

#define CONTEXT_BINDINGS \
    {"setup", (PyCFunction)SetupContext, METH_FASTCALL, NULL}, \
    {"teardown", (PyCFunction)TeardownContext, METH_FASTCALL, NULL}, \
    {"swap_buffers", (PyCFunction)SwapBuffers, METH_FASTCALL, NULL}, \
    {"clear", (PyCFunction)Clear, METH_FASTCALL, NULL},


PYTHON_API(SetupContext);
PYTHON_API(TeardownContext);
PYTHON_API(SwapBuffers);
PYTHON_API(Clear);
