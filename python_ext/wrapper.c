#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*

#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, fclose, fopen, fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*

#include "CoverageTable.h"  // CoverageTable*
#include "MNVTable.h"       // MNVTable*
#include "SequenceTable.h"  // SequenceTable*
#include "SNVTable.h"       // SNVTable*
#include "utils.h"          // sample_set


static PyObject*
coverage_from_file(PyObject* const restrict self,
                   PyObject* const restrict args)
{
    (void) self;

    char const* restrict path = NULL;
    int sample_id = 0;
    CoverageTableObject* restrict cov = NULL;

    if (!PyArg_ParseTuple(args, "siO!:coverage_from_file", &path, &sample_id, &CoverageTable, &cov))
    {
        return NULL;
    } // if

    errno = 0;
    FILE* restrict stream = fopen(path, "r");
    if (NULL == stream)
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_coverage_from_file(stream, cov->table, sample_id);
    Py_END_ALLOW_THREADS

    errno = 0;
    if (0 != fclose(stream))
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    return Py_BuildValue("i", count);
} // coverage_from_file


static PyObject*
variants_from_file(PyObject* const restrict self,
                   PyObject* const restrict args)
{
    (void) self;

    char const* restrict path = NULL;
    int sample_id = 0;
    SNVTableObject* restrict snv = NULL;
    MNVTableObject* restrict mnv = NULL;
    SequenceTableObject* restrict seq = NULL;

    if (!PyArg_ParseTuple(args, "siO!O!O!:variants_from_file", &path, &sample_id, &SNVTable, &snv, &MNVTable, &mnv, &SequenceTable, &seq))
    {
        return NULL;
    } // if

    errno = 0;
    FILE* restrict stream = fopen(path, "r");
    if (NULL == stream)
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_variants_from_file(stream, snv->table, mnv->table, seq->table, sample_id);
    Py_END_ALLOW_THREADS

    errno = 0;
    if (0 != fclose(stream))
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    return Py_BuildValue("i", count);
} // variants_from_file


static PyObject*
annotate_from_file(PyObject* const restrict self,
                   PyObject* const restrict args)
{
    (void) self;

    char const* restrict in_path = NULL;
    char const* restrict out_path = NULL;
    CoverageTableObject* restrict cov = NULL;
    SNVTableObject* restrict snv = NULL;
    MNVTableObject* restrict mnv = NULL;
    SequenceTableObject* restrict seq = NULL;
    PyObject* restrict list = NULL;

    if (!PyArg_ParseTuple(args, "ssO!O!O!O!|O!:annotate_from_file", &out_path, &in_path, &CoverageTable, &cov, &SNVTable, &snv, &MNVTable, &mnv, &SequenceTable, &seq, &PyList_Type, &list))
    {
        return NULL;
    } // if

    errno = 0;
    FILE* restrict istream = fopen(in_path, "r");
    if (NULL == istream)
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    errno = 0;
    FILE* restrict ostream = fopen(out_path, "w");
    if (NULL == ostream)
    {
        fclose(istream);
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    vrd_AVL_Tree* restrict subset = NULL;
    if (NULL != list)
    {
        subset = sample_set(list);
        if (NULL == subset)
        {
            fclose(istream);
            fclose(ostream);
            return NULL;
        } // if
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_annotate_from_file(ostream, istream, cov->table, snv->table, mnv->table, seq->table, subset);
    Py_END_ALLOW_THREADS

    errno = 0;
    if (0 != fclose(istream))
    {
        fclose(ostream);
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    errno = 0;
    if (0 != fclose(ostream))
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    return Py_BuildValue("i", count);
} // annotate_from_file


static PyMethodDef methods[] =
{
    {"coverage_from_file", (PyCFunction) coverage_from_file, METH_VARARGS,
     "coverage_from_file(path, sample_id, cov_table)\n"},

    {"variants_from_file", (PyCFunction) variants_from_file, METH_VARARGS,
     "variants_from_file(path, sample_id, snv_table, mnv_table, seq_table)\n"},

    {"annotate_from_file", (PyCFunction) annotate_from_file, METH_VARARGS,
     "annotate_from_file(out_path, in_path, cov_table, snv_table, mnv_table, seq_table[, subset])\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // methods


static struct PyModuleDef module =
{
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cvarda",
    .m_doc = "Python module around the Varda2 C library",
    .m_size = -1,
    .m_methods = methods
}; // module


PyMODINIT_FUNC
PyInit_cvarda(void)
{
    if (0 > PyType_Ready(&CoverageTable))
    {
        return NULL;
    } // if

    if (0 > PyType_Ready(&MNVTable))
    {
        return NULL;
    } // if

    if (0 > PyType_Ready(&SequenceTable))
    {
        return NULL;
    } // if

    if (0 > PyType_Ready(&SNVTable))
    {
        return NULL;
    } // if

    PyObject* const restrict mod = PyModule_Create(&module);
    if (NULL == mod)
    {
        return NULL;
    } // if

    int major = 0;
    int minor = 0;
    int patch = 0;
    vrd_version(&major, &minor, &patch);

    if (0 > PyModule_AddIntConstant(mod, "VERSION_MAJOR", major))
    {
        return NULL;
    } // if
    if (0 > PyModule_AddIntConstant(mod, "VERSION_MINOR", minor))
    {
        return NULL;
    } // if
    if (0 > PyModule_AddIntConstant(mod, "VERSION_PATCH", patch))
    {
        return NULL;
    } // if

    Py_INCREF(&CoverageTable);
    if (0 > PyModule_AddObject(mod, "CoverageTable", (PyObject*) &CoverageTable))
    {
        return NULL;
    } // if

    Py_INCREF(&MNVTable);
    if (0 > PyModule_AddObject(mod, "MNVTable", (PyObject*) &MNVTable))
    {
        return NULL;
    } // if

    Py_INCREF(&SequenceTable);
    if (0 > PyModule_AddObject(mod, "SequenceTable", (PyObject*) &SequenceTable))
    {
        return NULL;
    } // if

    Py_INCREF(&SNVTable);
    if (0 > PyModule_AddObject(mod, "SNVTable", (PyObject*) &SNVTable))
    {
        return NULL;
    } // if

    return mod;
} // PyInit_cvarda


int
main(int argc, char* argv[])
{
    (void) argc;

    wchar_t* const restrict program = Py_DecodeLocale(argv[0], NULL);
    if (NULL == program)
    {
        fprintf(stderr, "Py_DecodeLocale(): cannot decode argv[0]\n");
        return EXIT_FAILURE;
    } // if

    int const ret = PyImport_AppendInittab("cvarda", PyInit_cvarda);
    if (-1 == ret)
    {
        PyMem_RawFree(program);
        fprintf(stderr, "PyImport_AppendInittab() failed\n");
        return EXIT_FAILURE;
    } // if

    Py_SetProgramName(program);
    Py_Initialize();

    PyMem_RawFree(program);

    return EXIT_SUCCESS;
} // main
