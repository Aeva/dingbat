#pragma once
#include "python_api.h"

#define PAINTER_BINDINGS \
    {"bind_attr_buffer", (PyCFunction)BindAttributeBuffer, METH_FASTCALL, NULL}, \
    {"bind_uniform_buffer", (PyCFunction)BindUniformBuffer, METH_FASTCALL, NULL}, \
    {"bind_draw_arrays", (PyCFunction)BindDrawArrays, METH_FASTCALL, NULL}, \
    {"batch_draw", (PyCFunction)BatchDraw, METH_FASTCALL, NULL},


PYTHON_API(BindAttributeBuffer);
PYTHON_API(BindUniformBuffer);
PYTHON_API(BindDrawArrays);
PYTHON_API(BatchDraw);
