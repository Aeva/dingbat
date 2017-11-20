

#include <Python.h>
#include "context.h"
#include "shaders.h"
#include "buffers.h"
#include "painter.h"




PyMethodDef ThroughputMethods[] = {
    CONTEXT_BINDINGS
    SHADER_BINDINGS
    BUFFER_BINDINGS
    PAINTER_BINDINGS
    {NULL, NULL, 0, NULL}
};




PyModuleDef ModuleDef = {
    PyModuleDef_HEAD_INIT,
    "dingbat",
    "",
    -1,
    ThroughputMethods
};




PyMODINIT_FUNC PyInit_dingbat(void)
{
    PyObject *Module = PyModule_Create(&ModuleDef);
    return Module;
}
