from distutils.core import setup, Extension

floydModule = Extension(
        'floyd',
        sources = [
                'Source/cplus.c',
                'Source/evaluate.c',
                'Source/floydmodule.c',
                'Source/format.c',
                'Source/moves.c',
                'Source/kpk.c',
                'Source/parse.c',
                'Source/search.c',
                'Source/ttable.c',
                'Source/uci.c',
                'Source/zobrist.c' ],
        extra_compile_args = ['-O3', '-std=c11', '-Wall', '-pedantic'],
        undef_macros = ['NDEBUG']
)

setup(
        name         = 'floyd',
        version      = '0.1a',
        description  = 'Chess engine study',
        author       = 'Marcel van Kervinck',
        author_email = 'marcelk@bitpit.net',
        url          = 'http://marcelk.net/floyd',
        ext_modules  = [floydModule])
