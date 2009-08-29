#include <qlua.h>
#include <latreal.h>
#include <latint.h>
#include <latrandom.h>
#include <latcomplex.h>
#include <qmp.h>

const char *mtnLatReal = "qcd.lattice.real";
const char *opLatReal = "qcd.lattice.real.op";

mLatReal *
qlua_newLatReal(lua_State *L)
{
    QDP_D_Real *v = QDP_D_create_R();
    mLatReal *hdr;

    if (v == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        v = QDP_D_create_R();
        if (v == 0)
            luaL_error(L, "not enough memory (QDP_Real)");
    }
    hdr = lua_newuserdata(L, sizeof (mLatReal));
    hdr->ptr = v;
    luaL_getmetatable(L, mtnLatReal);
    lua_setmetatable(L, -2);

    return hdr;
}

mLatReal *
qlua_checkLatReal(lua_State *L, int idx)
{
    void *v = luaL_checkudata(L, idx, mtnLatReal);

    luaL_argcheck(L, v != 0, idx, "qcd.LatReal expected");

    return v;
}

static int
qLatReal_fmt(lua_State *L)
{
    char fmt[72];
    mLatReal *b = qlua_checkLatReal(L, 1);

    sprintf(fmt, "LatReal(%p)", b->ptr);
    lua_pushstring(L, fmt);

    return 1;
}

static int
qLatReal_gc(lua_State *L)
{
    mLatReal *b = qlua_checkLatReal(L, 1);

    QDP_D_destroy_R(b->ptr);
    b->ptr = 0;

    return 0;
}

static int
q_neg_R(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *res = qlua_newLatReal(L);
    QLA_D_Real m1 = -1;

    QDP_D_R_eq_r_times_R(res->ptr, &m1, a->ptr, QDP_all);
    return 1;
}

static int
q_R_sum(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    QLA_D_Real sum;

    QDP_D_r_eq_sum_R(&sum, a->ptr, QDP_all);
    lua_pushnumber(L, sum);

    return 1;
}

static int
q_R_norm2(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    QLA_D_Real n;

    QDP_D_r_eq_norm2_R(&n, a->ptr, QDP_all);
    lua_pushnumber(L, n);

    return 1;
}

static int
q_R_shift(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    QDP_Shift shift = qlua_checkShift(L, 2);
    QDP_ShiftDir dir = qlua_checkShiftDir(L, 3);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_sR(r->ptr, a->ptr, shift, dir, QDP_all);

    return 1;
}

static int
q_R_sin(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_sin_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_cos(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_cos_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_tan(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_tan_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_asin(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_asin_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_acos(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_acos_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_atan(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_atan_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_sqrt(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_sqrt_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_abs(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_fabs_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_exp(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_exp_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_expi(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatComplex *r = qlua_newLatComplex(L);

    QDP_D_C_eq_cexpi_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_log(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_log_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_sign(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_sign_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_floor(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_floor_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_ceil(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_ceil_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_sinh(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_sinh_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_cosh(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_cosh_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_tanh(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_tanh_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_log10(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_log10_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

int
q_R_random(lua_State *L)
{
    mLatRandom *a = qlua_checkLatRandom(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_random_S(r->ptr, a->ptr, QDP_all);

    return 1;
}

int
q_R_gaussian(lua_State *L)
{
    mLatRandom *a = qlua_checkLatRandom(L, 1);
    mLatReal *r = qlua_newLatReal(L);

    QDP_D_R_eq_gaussian_S(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_trunc(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatInt *r = qlua_newLatInt(L);

    QDP_D_I_eq_trunc_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
q_R_round(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatInt *r = qlua_newLatInt(L);

    QDP_D_I_eq_round_R(r->ptr, a->ptr, QDP_all);

    return 1;
}

static int
qLatReal_get(lua_State *L)
{
    switch (qlua_gettype(L, 2)) {
    case qTable: {
        mLatReal *V = qlua_checkLatReal(L, 1);
        QLA_D_Real *locked;
        int *idx = 0;
        double z;

        idx = qlua_checklatcoord(L, 2);
        locked = QDP_D_expose_R(V->ptr);
        if (QDP_node_number(idx) == QDP_this_node) {
            z = QLA_D_elem_R(locked[QDP_index(idx)]);
        } else {
            z = 0;
        }
        QDP_D_reset_R(V->ptr);
        qlua_free(L, idx);
        QMP_sum_double(&z);
        lua_pushnumber(L, z);

        return 1;
    }
    case qString:
        return qlua_lookup(L, 2, opLatReal);
    }
    return luaL_error(L, "bad index");
}


static int
qLatReal_put(lua_State *L)
{
    mLatReal *V = qlua_checkLatReal(L, 1);
    QLA_D_Real *locked;
    int *idx = 0;
    double z = luaL_checknumber(L, 3);

    idx = qlua_checklatcoord(L, 2);
    locked = QDP_D_expose_R(V->ptr);
    if (QDP_node_number(idx) == QDP_this_node) {
        QLA_D_elem_R(locked[QDP_index(idx)]) = z;
    }
    QDP_D_reset_R(V->ptr);
    qlua_free(L, idx);

    return 0;
}

int
q_R_add_R(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *b = qlua_checkLatReal(L, 2);
    mLatReal *c = qlua_newLatReal(L);

    QDP_D_R_eq_R_plus_R(c->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

int
q_R_sub_R(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *b = qlua_checkLatReal(L, 2);
    mLatReal *c = qlua_newLatReal(L);

    QDP_D_R_eq_R_minus_R(c->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

int
q_R_mul_R(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *b = qlua_checkLatReal(L, 2);
    mLatReal *c = qlua_newLatReal(L);

    QDP_D_R_eq_R_times_R(c->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

int
q_r_mul_R(lua_State *L)
{
    QLA_D_Real b = luaL_checknumber(L, 1);
    mLatReal *a = qlua_checkLatReal(L, 2);
    mLatReal *c = qlua_newLatReal(L);

    QDP_D_R_eq_r_times_R(c->ptr, &b, a->ptr, QDP_all);

    return 1;
}

int
q_R_mul_r(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    QLA_D_Real b = luaL_checknumber(L, 2);
    mLatReal *c = qlua_newLatReal(L);

    QDP_D_R_eq_r_times_R(c->ptr, &b, a->ptr, QDP_all);

    return 1;
}

int
q_R_div_R(lua_State *L)
{
    mLatReal *a = qlua_checkLatReal(L, 1);
    mLatReal *b = qlua_checkLatReal(L, 2);
    mLatReal *c = qlua_newLatReal(L);

    QDP_D_R_eq_R_divide_R(c->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

static int
q_latreal(lua_State *L)
{
    switch (qlua_gettype(L, 1)) {
    case qReal: {
        QLA_D_Real d = luaL_checknumber(L, 1);
        mLatReal *v = qlua_newLatReal(L);

        QDP_D_R_eq_r(v->ptr, &d, QDP_all);
        break;
    }
    case qLatInt: {
        mLatInt *d = qlua_checkLatInt(L, 1);
        mLatReal *v = qlua_newLatReal(L);

        QDP_D_R_eq_I(v->ptr, d->ptr, QDP_all);
        break;
    }
    case qLatReal: {
        mLatReal *d = qlua_checkLatReal(L, 1);
        mLatReal *v = qlua_newLatReal(L);
        
        QDP_D_R_eq_R(v->ptr, d->ptr, QDP_all);
        break;
    }
    default:
        return luaL_error(L, "bad argument");
    }
    return 1;
}

static const struct luaL_Reg LatRealMethods[] = {
    { "sum",       q_R_sum      },
    { "norm2",     q_R_norm2    },
    { "shift",     q_R_shift    },
    { "sin",       q_R_sin      },
    { "cos",       q_R_cos      },
    { "tan",       q_R_tan      },
    { "asin",      q_R_asin     },
    { "acos",      q_R_acos     },
    { "atan",      q_R_atan     },
    { "sqrt",      q_R_sqrt     },
    { "abs",       q_R_abs      },
    { "exp",       q_R_exp      },
    { "log",       q_R_log      },
    { "sign",      q_R_sign     },
    { "ceil",      q_R_ceil     },
    { "floor",     q_R_floor    },
    { "sinh",      q_R_sinh     },
    { "consh",     q_R_cosh     },
    { "tanh",      q_R_tanh     },
    { "log10",     q_R_log10    },
    { "expi",      q_R_expi     },
    { "trunc",     q_R_trunc    },
    { "round",     q_R_round    },
    { NULL,        NULL         }
};

static struct luaL_Reg mtLatReal[] = {
    { "__tostring",   qLatReal_fmt },
    { "__gc",         qLatReal_gc },
    { "__index",      qLatReal_get },
    { "__newindex",   qLatReal_put },
    { "__umn",        q_neg_R },
    { "__add",        qlua_add },
    { "__sub",        qlua_sub },
    { "__mul",        qlua_mul },
    { "__div",        qlua_div },
    { NULL,           NULL }
};

static struct luaL_Reg fLatReal[] = {
    { "Real",   q_latreal },
    { NULL,         NULL }
};

int
init_latreal(lua_State *L)
{
    luaL_register(L, qcdlib, fLatReal);
    qlua_metatable(L, mtnLatReal, mtLatReal);
    qlua_metatable(L, opLatReal, LatRealMethods);

    return 0;
}

int
fini_latreal(lua_State *L)
{
    return 0;
}