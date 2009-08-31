#include <qlua.h>
#include <string.h>
#include <qcomplex.h>
#include <qgamma.h>
#include <qvector.h>
#include <latint.h>
#include <latrandom.h>
#include <latreal.h>
#include <latcomplex.h>
#include <latcolvec.h>
#include <latcolmat.h>
#include <latdirferm.h>
#include <latdirprop.h>
#include <modules.h>
/* ZZZ include other package headers here */

const char *progname = "qlua";
const char *qcdlib = "qcd";

static struct {
    char *name;
    char *value;
} versions[] = {
    {"qlua",  "$Id$"},
    {"lua",    LUA_VERSION },
    {"qdp",    QDP_VERSION },
    {NULL,     NULL}
};

/* reporting */
void
message(const char *fmt, ...)
{
    if (QDP_this_node == 0) {
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
    }
}

void
report(lua_State *L, const char *fname, int status)
{
    if (QDP_this_node == 0) {
        if (status && !lua_isnil(L, -1)) {
            const char *msg = lua_tostring(L, -1);
            if (msg == NULL) msg = "(error object is not a string)";
            message("%s ERROR:: %s\n", progname, msg);
            lua_pop(L, 1);
        }
    }
}

/* memory allocation */
void *
qlua_malloc(lua_State *L, int size)
{
    void *p = malloc(size);
    if (p == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        p = malloc(size);
        if (p == 0)
            luaL_error(L, "not enough memory");
    }
    return p;
}

void
qlua_free(lua_State *L, void *ptr)
{
    if (ptr)
        free(ptr);
}

void
qlua_metatable(lua_State *L, const char *name, const luaL_Reg *table)
{
    int i;
    int base = lua_gettop(L);

    luaL_newmetatable(L, name);
    luaL_getmetatable(L, name);
    lua_pushstring(L, "__index");
    luaL_getmetatable(L, name);
    lua_settable(L, -3);
    for (i = 0; table[i].func; i++) {
        lua_pushstring(L, table[i].name);
        lua_pushcfunction(L, table[i].func);
        lua_settable(L, -3);
    }
    lua_settop(L, base);
}

int
qlua_lookup(lua_State *L, int idx, const char *table)
{
    const char *key = lua_tostring(L, idx);

    luaL_getmetatable(L, table);
    lua_getfield(L, -1, key);
    if (lua_isnil(L, -1))
        return luaL_error(L, "bad index");
    return 1;
}

int
qlua_index(lua_State *L, int n, const char *name, int max_value)
{
    int v = -1;

    luaL_checktype(L, n, LUA_TTABLE);
    lua_getfield(L, n, name);
    if (lua_isnumber(L, -1)) {
        v = luaL_checkint(L, -1);
        if ((v < 0) || (v >= max_value))
            v = -1;
    }
    lua_pop(L, 1);
    
    return v;
}

int
qlua_checkindex(lua_State *L, int n, const char *name, int max_value)
{
    int v = qlua_index(L, n, name, max_value);

    if (v == -1)
        luaL_error(L, "bad index");

    return v;
}

int
qlua_diracindex(lua_State *L, int n)
{
    return qlua_index(L, n, "d", QDP_Nf);
}

int
qlua_checkdiracindex(lua_State *L, int n)
{
    return qlua_checkindex(L, n, "d", QDP_Nf);
}

int
qlua_colorindex(lua_State *L, int n)
{
    return qlua_index(L, n, "c", QDP_Nc);
}

int
qlua_checkcolorindex(lua_State *L, int n)
{
    return qlua_checkindex(L, n, "c", QDP_Nc);
}

int
qlua_leftindex(lua_State *L, int n)
{
    return qlua_index(L, n, "a", QDP_Nc);
}

int
qlua_checkleftindex(lua_State *L, int n)
{
    return qlua_checkindex(L, n, "a", QDP_Nc);
}

int
qlua_rightindex(lua_State *L, int n)
{
    return qlua_index(L, n, "b", QDP_Nc);
}

int
qlua_checkrightindex(lua_State *L, int n)
{
    return qlua_checkindex(L, n, "b", QDP_Nc);
}

int
qlua_gammaindex(lua_State *L, int n)
{
    int d = qlua_index(L, n, "mu", 6);

    if (d == 4)
        return -1;
    return d;
}

int
qlua_checkgammaindex(lua_State *L, int n)
{
    int d = qlua_gammaindex(L, n);

    if (d == -1)
        return luaL_error(L, "bad index");

    return d;
}

int
qlua_gammabinary(lua_State *L, int n)
{
    return qlua_index(L, n, "n", 16);
}

int
qlua_checkgammabinary(lua_State *L, int n)
{
    return qlua_checkindex(L, n, "n", 16);
}

static int
qlua_type(lua_State *L, int idx, const char *mt)
{
    int base = lua_gettop(L);
    int v;

    lua_getmetatable(L, idx);
    luaL_getmetatable(L, mt);
    v = lua_equal(L, -1, -2);
    lua_settop(L, base);

    return v;
}

int
qlua_gettype(lua_State *L, int idx)
{
    luaL_checkany(L, idx);
    switch (lua_type(L, idx)) {
    case LUA_TNUMBER:
        return qReal;
    case LUA_TSTRING:
        return qString;
    case LUA_TTABLE:
        return qTable;
    case LUA_TUSERDATA: {
        static const struct {
            const char *name;
            int ty;
        } t[] = {
            { mtnComplex,       qComplex },
            { mtnGamma,         qGamma },
            { mtnVecInt,        qVecInt },
            { mtnVecDouble,     qVecDouble },
            { mtnVecComplex,    qVecComplex },
            { mtnLatInt,        qLatInt },
            { mtnLatReal,       qLatReal },
            { mtnLatRandom,     qLatRandom },
            { mtnLatComplex,    qLatComplex },
            { mtnLatColVec,     qLatColVec },
            { mtnLatColMat,     qLatColMat },
            { mtnLatDirFerm,    qLatDirFerm },
            { mtnLatDirProp,    qLatDirProp },
            /* ZZZ other types */
            { NULL,              qOther }
        };
        int i;

        for (i = 0; t[i].name; i++)
            if (qlua_type(L, idx, t[i].name)) return t[i].ty;
        return qOther;
    }
    default:
        return qOther;
    }
}

/* generic operations dispatchers */
#define Op1Idx(a)   (a)
#define Op2Idx(a,b) ((a)*(qOther + 1) + (b))

static q_op qt_add[(qOther + 1) * (qOther + 1)];

void
qlua_reg_add(int ta, int tb, q_op op)
{
    qt_add[Op2Idx(ta, tb)] = op;
}

int 
qlua_add(lua_State *L)
{
    q_op op = qt_add[Op2Idx(qlua_gettype(L, 1), qlua_gettype(L, 2))];

    if (op)
        return op(L);
    else
        return luaL_error(L, "bad argument for addition");
}

static q_op qt_sub[(qOther + 1) * (qOther + 1)];

void
qlua_reg_sub(int ta, int tb, q_op op)
{
    qt_sub[Op2Idx(ta, tb)] = op;
}

int 
qlua_sub(lua_State *L)
{
    q_op op = qt_sub[Op2Idx(qlua_gettype(L, 1), qlua_gettype(L, 2))];

    if (op)
        return op(L);
    else
        return luaL_error(L, "bad argument for subtraction");
}

static q_op qt_mul[(qOther + 1) * (qOther + 1)];

void
qlua_reg_mul(int ta, int tb, q_op op)
{
    qt_mul[Op2Idx(ta, tb)] = op;
}

int 
qlua_mul(lua_State *L)
{
    q_op op = qt_mul[Op2Idx(qlua_gettype(L, 1), qlua_gettype(L, 2))];

    if (op)
        return op(L);
    else
        return luaL_error(L, "bad argument for multiplication");
}

static q_op qt_div[(qOther + 1) * (qOther + 1)];

void
qlua_reg_div(int ta, int tb, q_op op)
{
    qt_div[Op2Idx(ta, tb)] = op;
}

int 
qlua_div(lua_State *L)
{
    q_op op = qt_div[Op2Idx(qlua_gettype(L, 1), qlua_gettype(L, 2))];

    if (op)
        return op(L);
    else
        return luaL_error(L, "bad argument for division");
}

static q_op qt_dot[(qOther + 1)];

void
qlua_reg_dot(int ta, q_op op)
{
    qt_dot[Op1Idx(ta)] = op;
}

static int 
q_dot(lua_State *L)
{
    q_op op = qt_dot[Op1Idx(qlua_gettype(L, 1))];

    if (op)
        return op(L);
    else
        return luaL_error(L, "bad argument for inner dot");
}

static struct luaL_Reg fQCD[] = {
    { "dot",    q_dot}, /* local inner dot */
    { NULL,     NULL}
};

/* environment setup */
void
qlua_init(lua_State *L)
{
    static const struct {
        int (*init)(lua_State *L);
    } qcd_inits[] = {
        { init_complex },
        { init_gamma },
        { init_vector },
        { init_latint },
        { init_latrandom },
        { init_latreal },
        { init_latcomplex },
        { init_latcolvec },
        { init_latcolmat },
        { init_latdirferm },
        { init_latdirprop },
        /* ZZZ add other packages here */
        { NULL }
    };

    int i;

    lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
    luaL_openlibs(L);  /* open libraries */
    luaL_register(L, qcdlib, fQCD);
    for (i = 0; qcd_inits[i].init; i++) {
        lua_pushcfunction(L, qcd_inits[i].init);
        lua_call(L, 0, 0);
    }
    lua_getglobal(L, qcdlib);
    lua_pushnumber(L, QDP_Nc);
    lua_setfield(L, -2, "Nc");
    lua_pushnumber(L, QDP_Nf);
    lua_setfield(L, -2, "Nf");
    lua_newtable(L);
    for (i = 0; versions[i].name; i++) {
        lua_pushstring(L, versions[i].value);
        lua_setfield(L, -2, versions[i].name);
    }
    lua_setfield(L, -2, "version");
    lua_pop(L, 1);
    lua_gc(L, LUA_GCRESTART, 0);
}

/* cleanup (mostly housekeeping to make various tools happy */
void
qlua_fini(lua_State *L)
{
    static struct {
        int (*fini)(lua_State *L);
    } qcd_finis[] = { /* keep it in the reverse order with respect to init */
        /* ZZZ add other packages here */
        { fini_latdirprop },
        { fini_latdirferm },
        { fini_latcolmat },
        { fini_latcolvec },
        { fini_latcomplex },
        { fini_latreal },
        { fini_latrandom },
        { fini_latint },
        { fini_vector },
        { fini_gamma },
        { fini_complex },
        { NULL }
    };
    int i;

    for (i = 0; qcd_finis[i].fini; i++) {
        lua_pushcfunction(L, qcd_finis[i].fini);
        lua_call(L, 0, 0);
    }
}

/* the driver */
int
main(int argc, char *argv[])
{
    int status = 1;
    int i;
    lua_State *L = NULL;

    if (QDP_initialize(&argc, &argv)) {
        fprintf(stderr, "QDP initialization failed\n");
        return 1;
    }
    L = lua_open();
    if (L == NULL) {
        message("can not create Lua state");
        goto end;
    }
    qlua_init(L);  /* open libraries */

    for (i = 1; i < argc; i++) {
        status = luaL_dofile(L, argv[i]);
        report(L, argv[i], status);
        if (status)
            break;
    }
    qlua_fini(L);
    lua_close(L);
end:
    QDP_finalize();
    return status;
}
