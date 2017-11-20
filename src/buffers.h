#pragma once
#include "python_api.h"

#define BUFFER_BINDINGS \
    {"create_buffer", (PyCFunction)WrapCreateBuffer, METH_FASTCALL, NULL}, \
    {"delete_buffer", (PyCFunction)WrapDeleteBuffer, METH_FASTCALL, NULL}, \
    {"fill_buffer", (PyCFunction)WrapFillBuffer, METH_FASTCALL, NULL}, \
    {"fill_uniform_block", (PyCFunction)WrapFillUniformBlock, METH_FASTCALL, NULL},


PYTHON_API(WrapCreateBuffer);
PYTHON_API(WrapDeleteBuffer);
PYTHON_API(WrapFillBuffer);
PYTHON_API(WrapFillUniformBlock);
