
/*----------------------------------------------------------------------+
 |                                                                      |
 |      cplus.h - a loose collection of small C extensions              |
 |                                                                      |
 +----------------------------------------------------------------------*/

// Safe definition of statement-like macros
#define _Statement(...) do{ __VA_ARGS__ }while(0)

#define arrayLen(a) (sizeof(a) / sizeof((a)[0]))

#define max(a, b) ((a) >= (b) ? (a) : (b))
#define min(a, b) ((a) <= (b) ? (a) : (b))

#define setMax(a, b) _Statement( if ((a) < (b)) (a) = (b); )
#define setMin(a, b) _Statement( if ((a) > (b)) (a) = (b); )

#define null      ((void*) 0)
#define constNull ((const void*) 0)

#define maxInt ((int)(~0U >> 1))
#define minInt (-maxInt - 1)

/*----------------------------------------------------------------------+
 |      Exceptions                                                      |
 +----------------------------------------------------------------------*/

struct xError {
        const char *format;
        const char *file;
        const char *function;
        int line;
        int argc;
        // struct xValue argv[];
};

typedef struct xError *err_t;

#define OK ((err_t) 0)

#define check(err) _Statement(\
        if ((err) != OK)      \
                goto cleanup; \
)

err_t err_free(err_t err);

#define xRaise(msg) _Statement(\
        static struct xError _static_err = {\
                .format = (msg),\
                .file = __FILE__,\
                .function = __func__,\
                .line = __LINE__,\
                .argc = -1\
        };\
        err = &_static_err;\
        goto cleanup;\
)

#define xAssert(cond) _Statement(\
        if (!(cond))\
                xRaise("Assertion (" #cond ") failed");\
)

/*----------------------------------------------------------------------+
 |      Pairs / Tuples                                                  |
 +----------------------------------------------------------------------*/

#define Tuple(type, n)\
struct {\
        type v[n];\
}

#define Pair(type)              Tuple(type, 2)

typedef Pair(int)               intPair;

/*----------------------------------------------------------------------+
 |      Lists                                                           |
 +----------------------------------------------------------------------*/

#define List(type)\
struct {\
        type *v;\
        int len;\
        int maxLen;\
}

#define emptyList { null, 0, 0 }

typedef List(unsigned char)      byteList;
typedef List(char)               charList;
typedef List(short)              shortList;
typedef List(int)                intList;
typedef List(long)               longList;
typedef List(unsigned long long) unsignedLongLongList;

#define initialListSize (128)

#define pushList(list, value) _Statement(\
        if ((list).len >= (list).maxLen) {\
                /* Avoid GCC warning "dereferencing type-punned pointer\
                   will break strict-aliasing rules" */\
                void *_v = (list).v;\
                err = list_ensure_len(\
                        &_v,\
                        &(list).maxLen,\
                        (list).len + 1,\
                        sizeof((list).v[0]),\
                        (initialListSize + sizeof((list).v[0]) - 1) / sizeof((list).v[0])\
                );\
                check(err);\
                (list).v = _v;\
        }\
        (list).v[(list).len++] = (value);\
)

#define popList(list) ((list).v[--(list).len])

#define freeList(list) _Statement(\
        if ((list).v) {           \
                free((list).v);   \
                (list).v = null;  \
                (list).len = 0;   \
                (list).maxLen = 0;\
        }\
)

err_t list_ensure_len(void **v, int *maxLen, int minLen, int unit, int newLen);

/*----------------------------------------------------------------------+
 |      Main support                                                    |
 +----------------------------------------------------------------------*/

int xExitMain(err_t err);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

