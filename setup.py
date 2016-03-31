from distutils.core import setup, Extension
import os

floydModule = Extension(
        'floyd',
        sources = [
                'Source/cplus.c',
                'Source/engine.c',
                'Source/evaluate.c',
                'Source/floydmodule.c',
                'Source/format.c',
                'Source/moves.c',
                'Source/kpk.c',
                'Source/parse.c',
                'Source/search.c',
                'Source/test.c',
                'Source/ttable.c',
                'Source/uci.c',
                'Source/zobrist.c' ],
        undef_macros = ['NDEBUG']
)

setup(
        name         = 'floyd',
        version      = os.environ['floydVersion'],
        description  = 'Chess engine study',
        author       = 'Marcel van Kervinck',
        author_email = 'marcelk@bitpit.net',
        url          = 'http://marcelk.net/floyd',
        ext_modules  = [floydModule])
