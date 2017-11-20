#pragma once
#include "python_api.h"

#define PAINTER_BINDINGS \
    {"bind_attr_buffer", (PyCFunction)WrapBindAttributeBuffer, METH_FASTCALL, NULL}, \
    {"bind_uniform_buffer", (PyCFunction)WrapBindUniformBuffer, METH_FASTCALL, NULL}, \
    {"bind_draw_arrays", (PyCFunction)WrapBindDrawArrays, METH_FASTCALL, NULL}, \
    {"batch_draw", (PyCFunction)WrapBatchDraw, METH_FASTCALL, NULL},


PYTHON_API(WrapBindAttributeBuffer);
PYTHON_API(WrapBindUniformBuffer);
PYTHON_API(WrapBindDrawArrays);
PYTHON_API(WrapBatchDraw);
