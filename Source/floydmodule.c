
/*----------------------------------------------------------------------+
 |                                                                      |
 |      floydmodule.c                                                   |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 2015, Marcel van Kervinck
 *  All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// Python include (must come first)
#include "Python.h"

// Standard C includes
#include <stdbool.h>

// Generic C extensions
#include "cplus.h"

// Other module includes
#include "Board.h"
#include "evaluate.h"
#include "Engine.h"
#include "stringCopy.h"

/*----------------------------------------------------------------------+
 |      Module                                                          |
 +----------------------------------------------------------------------*/

// Module docstring
PyDoc_STRVAR(floyd_doc,
        "Chess engine study.");

/*----------------------------------------------------------------------+
 |      evaluate(...)                                                   |
 +----------------------------------------------------------------------*/

PyDoc_STRVAR(evaluate_doc,
        "evaluate(fen) -> score\n"
);

static PyObject *
floydmodule_evaluate(PyObject *self, PyObject *args)
{
        char *fen;

        if (!PyArg_ParseTuple(args, "s", &fen))
                return NULL;

        struct board board;
        int len = setupBoard(&board, fen);
        if (len <= 0)
                return PyErr_Format(PyExc_ValueError, "Invalid FEN (%s)", fen);

        int score = evaluate(&board);

        return PyFloat_FromDouble(score / 1000.0);
}

/*----------------------------------------------------------------------+
 |      setCoefficient(...)                                             |
 +----------------------------------------------------------------------*/

// TODO: create a proper object for the vector
// TODO: create coef name list

PyDoc_STRVAR(setCoefficient_doc,
        "setCoefficient(coef, newValue) -> oldValue\n"
        "\n"
        "!!! EXPERIMENTAL !!!\n"
);

static PyObject *
floydmodule_setCoefficient(PyObject *self, PyObject *args)
{
        int coef, newValue;

        if (!PyArg_ParseTuple(args, "ii", &coef, &newValue))
                return NULL;

        if (coef < 0 || coef >= vectorLen)
                return PyErr_Format(PyExc_IndexError, "coef %d out of range", coef);

        long oldValue = globalVector[coef];
        globalVector[coef] = newValue;

        PyObject *result = PyTuple_New(2);
        if (!result)
                return NULL;

        if (PyTuple_SetItem(result, 0, PyInt_FromLong(oldValue))) {
                Py_DECREF(result);
                return NULL;
        }

        if (PyTuple_SetItem(result, 1, PyString_FromString(vectorLabels[coef]))) {
                Py_DECREF(result);
                return NULL;
        }

        return result;
}

/*----------------------------------------------------------------------+
 |      search(...)                                                     |
 +----------------------------------------------------------------------*/

PyDoc_STRVAR(search_doc,
        "search(fen, depth) -> score, pv\n"
);

static PyObject *
floydmodule_search(PyObject *self, PyObject *args)
{
        char *fen;
        int depth;

        if (!PyArg_ParseTuple(args, "si", &fen, &depth))
                return NULL;

        struct board board;
        int len = setupBoard(&board, fen);
        if (len <= 0)
                return PyErr_Format(PyExc_ValueError, "Invalid FEN (%s)", fen);

        board.eloDiff = atoi(fen + len);

        intList pv = emptyList;
        int pvLen = 0;
        while (pv.v[pvLen]) pvLen++;

        int score = rootSearch(&board, depth, null, null);

        PyObject *result = PyTuple_New(2);
        if (!result) {
                freeList(pv);
                return NULL;
        }

        if (PyTuple_SetItem(result, 0, PyFloat_FromDouble(score / 1000.0))) {
                Py_DECREF(result);
                freeList(pv);
                return NULL;
        }

        PyObject *list = PyList_New(pvLen);
        if (!list) {
                Py_DECREF(result);
                freeList(pv);
                return NULL;
        }

        for (int i=0; i<pvLen; i++) {
                // TODO: make a moveString in required notation
                if (PyList_SetItem(list, i, PyInt_FromLong(pv.v[i]))) {
                        Py_DECREF(list);
                        Py_DECREF(result);
                        freeList(pv);
                        return NULL;
                }
        }

        if (PyTuple_SetItem(result, 1, list)) {
                Py_DECREF(list);
                Py_DECREF(result);
                freeList(pv);
                return NULL;
        }

        freeList(pv);

        return result;
}

/*----------------------------------------------------------------------+
 |      Method table                                                    |
 +----------------------------------------------------------------------*/

static PyMethodDef floydMethods[] = {
	{ "evaluate", floydmodule_evaluate,           METH_VARARGS,               evaluate_doc },
	{ "setCoefficient", floydmodule_setCoefficient, METH_VARARGS,             setCoefficient_doc },
	{ "search",   floydmodule_search,             METH_VARARGS,               search_doc },
	{ NULL, }
};

/*----------------------------------------------------------------------+
 |      initfloyd                                                       |
 +----------------------------------------------------------------------*/

PyMODINIT_FUNC
initfloyd(void)
{
	PyObject *module;

        // Create the module and add the functions
        module = Py_InitModule3("floyd", floydMethods, floyd_doc);
        if (module == NULL)
                return;

        // Add startPosition as a string constant
        if (PyModule_AddStringConstant(module, "startPosition", startpos))
                return;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

