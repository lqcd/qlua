#include <qlua.h>                                                    /* DEPS */
#include <qcomplex.h>                                                /* DEPS */
#include <qvector.h>                                                 /* DEPS */
#include <string.h>

const char mtnVecInt[]     = "qcd.mtVecInt";
const char mtnVecReal[]    = "qcd.mtVecReal";
const char mtnVecComplex[] = "qcd.mtVecComplex";

static char vector_ns[] = "vector";

#define VSIZE(s,n,t) (sizeof (s) + ((n)-1)*sizeof (t))

static void *
qlua_checkVector(lua_State *L, int idx, const char *mt)
{
    void *v = luaL_checkudata(L, idx, mt);

    luaL_argcheck(L, v != NULL, idx, "qcd.vect_* expected");

    return v;
}

tVecInt *
qlua_checkVecInt(lua_State *L, int idx)
{
    return (tVecInt *)qlua_checkVector(L, idx, mtnVecInt);
}

tVecReal *
qlua_checkVecReal(lua_State *L, int idx)
{
    return (tVecReal *)qlua_checkVector(L, idx, mtnVecReal);
}

tVecComplex *
qlua_checkVecComplex(lua_State *L, int idx)
{
    return (tVecComplex *)qlua_checkVector(L, idx, mtnVecComplex);
}

static void *
qlua_newVector(lua_State *L, int s, const char *mt)
{
    void *v = lua_newuserdata(L, s);

    luaL_getmetatable(L, mt);
    lua_setmetatable(L, -2);
    
    return v;
}

tVecInt *
qlua_newVecInt(lua_State *L, int size)
{
    return qlua_newVector(L, VSIZE(tVecInt, size, QLA_Int),
                          mtnVecInt);
}

tVecReal *
qlua_newVecReal(lua_State *L, int size)
{
    return qlua_newVector(L, VSIZE(tVecReal, size, QLA_Real), mtnVecReal);
}

tVecComplex *
qlua_newVecComplex(lua_State *L, int size)
{
    return qlua_newVector(L, VSIZE(tVecComplex, size, QLA_Complex),
                          mtnVecComplex);
}

/* integer vectors */
static int
vi_fmt(lua_State *L)                                           /* (-1,+1,e) */
{
    char fmt[72];
    tVecInt *v = qlua_checkVecInt(L, 1);

    sprintf(fmt, "vector.int[%d]", v->size);
    lua_pushstring(L, fmt);

    return 1;
}

static int
vi_get(lua_State *L)                                           /* (-2,+1,e) */
{
    tVecInt *v = qlua_checkVecInt(L, 1);
    int i = luaL_checkint(L, 2);

    if ((i >= 0) && (i < v->size)) {
        lua_pushnumber(L, v->val[i]);
        return 1;
    }

    return qlua_badindex(L, "vector.int[]");
}

static int
vi_size(lua_State *L)
{
    tVecInt *v = qlua_checkVecInt(L, 1);

    lua_pushnumber(L, v->size);

    return 1;
}

static int
vi_put(lua_State *L)                                           /* (-3,+0,e) */
{
    tVecInt *v = qlua_checkVecInt(L, 1);
    int idx = luaL_checkint(L, 2);
    int x = luaL_checkint(L, 3);

    if ((idx >= 0) && (idx < v->size)) {
        v->val[idx] = x;
        return 0;
    }
    return qlua_badindex(L, "vector.int[]");
}

static int
v_int(lua_State *L)                                            /* (-1,+1,e) */
{
    int s = luaL_checkint(L, 1);
    tVecInt *v = qlua_newVecInt(L, s);
    v->size = s;
    
    return 1;
}

/* real vectors */
static int
vd_fmt(lua_State *L)                                           /* (-1,+1,e) */
{
    char fmt[72];
    tVecReal *v = qlua_checkVecReal(L, 1);

    sprintf(fmt, "vector.real[%d]", v->size);
    lua_pushstring(L, fmt);

    return 1;
}

static int
vd_get(lua_State *L)                                           /* (-2,+1,e) */
{
    tVecReal *v = qlua_checkVecReal(L, 1);
    int i = luaL_checkint(L, 2);

    if ((i >= 0) && (i < v->size)) {
        lua_pushnumber(L, v->val[i]);
        return 1;
    }

    return qlua_badindex(L, "vector.real[]");
}

static int
vd_size(lua_State *L)
{
    tVecReal *v = qlua_checkVecReal(L, 1);

    lua_pushnumber(L, v->size);

    return 1;
}

static int
vd_put(lua_State *L)                                           /* (-3,+0,e) */
{
    tVecReal *v = qlua_checkVecReal(L, 1);
    int idx = luaL_checkint(L, 2);
    double x = luaL_checknumber(L, 3);

    if ((idx >= 0) && (idx < v->size)) {
        v->val[idx] = x;
        return 0;
    }
    return qlua_badindex(L, "vector.real[]");
}

static int
v_real(lua_State *L)                                         /* (-1,+1,e) */
{
    int s = luaL_checkint(L, 1);
    tVecReal *v = qlua_newVecReal(L, s);
    v->size = s;
    
    return 1;
}

/* complex vectors */
static int
vc_fmt(lua_State *L)                                           /* (-1,+1,e) */
{
    char fmt[72];
    tVecComplex *v = qlua_checkVecComplex(L, 1);

    sprintf(fmt, "vector.complex[%d]", v->size);
    lua_pushstring(L, fmt);

    return 1;
}

static int
vc_get(lua_State *L)                                           /* (-2,+1,e) */
{
    tVecComplex *v = qlua_checkVecComplex(L, 1);
    int i = luaL_checkint(L, 2);

    if ((i >= 0) && (i < v->size)) {
        QLA_Complex *z = qlua_newComplex(L);
        QLA_c_eq_c(*z, v->val[i]);
        return 1;
    }

    return qlua_badindex(L, "vector.complex[]");
}

static int
vc_size(lua_State *L)
{
    tVecComplex *v = qlua_checkVecComplex(L, 1);

    lua_pushnumber(L, v->size);

    return 1;
}

static int
vc_put(lua_State *L)                                           /* (-3,+0,e) */
{
    tVecComplex *v = qlua_checkVecComplex(L, 1);
    int idx = luaL_checkint(L, 2);

    if ((idx >= 0) && (idx < v->size)) {
        switch (qlua_gettype(L, 3)) {
        case qReal: {
            double x = luaL_checknumber(L, 3);
            QLA_real(v->val[idx]) = x;
            QLA_imag(v->val[idx]) = 0;
            return 0;
        }
        case qComplex: {
            QLA_Complex *z = qlua_checkComplex(L, 3);
            QLA_c_eq_c(v->val[idx], *z);
            return 0;
        }
        }
    }

    return qlua_badindex(L, "vector.real[]");
}

static int
v_complex(lua_State *L)                                         /* (-1,+1,e) */
{
    int s = luaL_checkint(L, 1);
    tVecComplex *v = qlua_newVecComplex(L, s);
    v->size = s;
    
    return 1;
}

static const luaL_Reg mtVecInt[] = {
    { "__tostring",     vi_fmt    },
    { "__index",        vi_get    },
    { "__newindex",     vi_put    },
    { "__len",          vi_size   },
    { NULL,             NULL      }
};

static const luaL_Reg mtVecReal[] = {
    { "__tostring",     vd_fmt    },
    { "__index",        vd_get    },
    { "__newindex",     vd_put    },
    { "__len",          vd_size   },
    { NULL,             NULL      }
};

static const luaL_Reg mtVecComplex[] = {
    { "__tostring",     vc_fmt    },
    { "__index",        vc_get    },
    { "__newindex",     vc_put    },
    { "__len",          vc_size   },
    { NULL,             NULL      }
};

/* vector constructors */
static const luaL_Reg fVector[] = {
    { "int",       v_int     },
    { "real",      v_real    },
    { "complex",   v_complex },
    { NULL,            NULL      }
};

int
init_vector(lua_State *L)
{
    luaL_register(L, vector_ns,      fVector);
    qlua_metatable(L, mtnVecInt,     mtVecInt);
    qlua_metatable(L, mtnVecReal,    mtVecReal);
    qlua_metatable(L, mtnVecComplex, mtVecComplex);
    return 0;
}

int
fini_vector(lua_State *L)
{
    return 0;
}
