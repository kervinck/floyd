from distutils.core import setup, Extension

module1 = Extension(
        'floyd',
        sources = [
                'Source/floydmodule.c',
                'Source/format.c',
                'Source/moves.c',
                'Source/polyglot.c',
                'Source/evaluate.c',
                'Source/kpk.c',
                'Source/search.c',
                'Source/uci.c',
                'Source/cplus.c' ],
        extra_compile_args = ['-O3', '-std=c11', '-Wall', '-pedantic'],
        undef_macros = ['NDEBUG']
)

setup(
        name         = 'floyd',
        version      = '1.0a',
        description  = 'Chess engine study',
        author       = 'Marcel van Kervinck',
        author_email = 'marcelk@bitpit.net',
        url          = 'http://marcelk.net/floyd',
        ext_modules  = [module1])
