#define PY_SSIZE_T_CLEAN 1
#include <Python.h>
#include <bytesobject.h>

#if PY_MAJOR_VERSION >= 3
#define PyInt_Check PyLong_Check
#define PyInt_AsLong PyLong_AsLong
#endif

#include "config.h"
#include "MTXMEM.H"
#include "BITIO.H"
#include "AHUFF.H"
#include "LZCOMP.H"
#include "ERRCODES.H"


static PyObject *LZCompError;

static PyObject* lzcomp_decompress(PyObject *self, PyObject *args) {

  uint8_t *input;
  size_t input_length;
  int ok;

  ok = PyArg_ParseTuple(args, "s#:decompress", &input, &input_length);
  if (!ok) {
    return NULL;
  }

  MTX_MemHandler *mem = MTX_mem_Create(malloc, realloc, free);
  LZCOMP *lzcomp = MTX_LZCOMP_Create1(mem);
  long output_length;
  uint8_t *output = NULL;
  output = (uint8_t *)MTX_LZCOMP_UnPackMemory(lzcomp, input, input_length, &output_length, 3);
  
  if (!output) {
    MTX_LZCOMP_Destroy(lzcomp);
    free(mem);
    PyErr_SetString(LZCompError, "MTX_LZCOMP_UnPackMemory failed");
    return NULL;
  }

  return PyBytes_FromStringAndSize((char*)output, output_length);
}

PyDoc_STRVAR(compress__doc__,
"compress(string) -- Returned compressed string.");

static PyObject* lzcomp_compress(PyObject *self, PyObject *args) {

  uint8_t *input;
  size_t input_length;
  int ok;

  ok = PyArg_ParseTuple(args, "s#:compress", &input, &input_length);
  if (!ok) {
    return NULL;
  }

  MTX_MemHandler *mem = MTX_mem_Create(malloc, realloc, free);
  LZCOMP *lzcomp = MTX_LZCOMP_Create1(mem);
  long output_length;
  uint8_t *output = NULL;
  output = (uint8_t *)MTX_LZCOMP_PackMemory(lzcomp, input, input_length, &output_length);
  
  if (!output) {
    MTX_LZCOMP_Destroy(lzcomp);
    free(mem);
    PyErr_SetString(LZCompError, "MTX_LZCOMP_PackMemory failed");
    return NULL;
  }

  return PyBytes_FromStringAndSize((char*)output, output_length);
}

PyDoc_STRVAR(decompress__doc__,
"decompress(string) -- Return decompressed string.");

static PyMethodDef lzcomp_methods[] = {
  {"compress",   lzcomp_compress,   METH_VARARGS, compress__doc__},
  {"decompress", lzcomp_decompress, METH_VARARGS, decompress__doc__},
  {NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(lzcomp__doc__,
"The functions in this module allow decompression using the LZCOMP algorithm.\n"
"\n"
"compress(string) -- Compress string.\n"
"decompress(string) -- Decompresses a compressed string.\n");

#if PY_MAJOR_VERSION >= 3
#define INIT_LZCOMP   PyInit_lzcomp
#define CREATE_LZCOMP PyModule_Create(&lzcomp_module)
#define RETURN_LZCOMP return m

static struct PyModuleDef lzcomp_module = {
  PyModuleDef_HEAD_INIT,
  "lzcomp",
  lzcomp__doc__,
  0,
  lzcomp_methods,
  NULL,
  NULL,
  NULL
};
#else
#define INIT_LZCOMP   initlzcomp
#define CREATE_LZCOMP Py_InitModule3("lzcomp", lzcomp_methods, lzcomp__doc__)
#define RETURN_LZCOMP return
#endif

PyMODINIT_FUNC INIT_LZCOMP(void) {
  PyObject *m = CREATE_LZCOMP;

  LZCompError = PyErr_NewException((char*) "lzcomp.error", NULL, NULL);

  if (LZCompError != NULL) {
    Py_INCREF(LZCompError);
    PyModule_AddObject(m, "error", LZCompError);
  }

  RETURN_LZCOMP;
}
