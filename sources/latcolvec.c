#include <qlua.h>
#include <latcolvec.h>
#include <qcomplex.h>
#include <latint.h>
#include <latcomplex.h>
#include <latrandom.h>
#include <qmp.h>

const char *mtnLatColVec = "qcd.lattice.colvec";
static const char *opLatColVec = "qcd.lattice.colvec.op";

mLatColVec *
qlua_newLatColVec(lua_State *L)
{
    QDP_D_ColorVector *v = QDP_D_create_V();
    mLatColVec *hdr;

    if (v == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        v = QDP_D_create_V();
        if (v == 0)
            luaL_error(L, "not enough memory (QDP_ColorVector)");
    }
    hdr = lua_newuserdata(L, sizeof (mLatColVec));
    hdr->ptr = v;
    luaL_getmetatable(L, mtnLatColVec);
    lua_setmetatable(L, -2);

    return hdr;
}

mLatColVec *
qlua_checkLatColVec(lua_State *L, int idx)
{
    void *v = luaL_checkudata(L, idx, mtnLatColVec);
    
    luaL_argcheck(L, v != 0, idx, "qcd.LatColVec expected");
    
    return v;
}

int
qLatColVec_fmt(lua_State *L)
{
    char fmt[72];
    mLatColVec *b = qlua_checkLatColVec(L, 1);

    sprintf(fmt, "LatColVec(%p)", b->ptr);
    lua_pushstring(L, fmt);

    return 1;
}

int
qLatColVec_gc(lua_State *L)
{
    mLatColVec *b = qlua_checkLatColVec(L, 1);

    QDP_D3_destroy_V(b->ptr);
    b->ptr = 0;

    return 0;
}

int
qLatColVec_get(lua_State *L)
{
    switch (qlua_gettype(L, 2)) {
    case qTable: {
        mLatColVec *V = qlua_checkLatColVec(L, 1);
        int c = qlua_checkcolorindex(L, 2, QDP_Nc);
        int *idx = qlua_latcoord(L, 2);

        if (idx == NULL) {
            mLatComplex *r = qlua_newLatComplex(L);
            
            QDP_D3_C_eq_elem_V(r->ptr, V->ptr, c, QDP_all);
        } else {
            QLA_D_Complex *W = qlua_newComplex(L);
            QLA_D_ColorVector *locked;
            double z_re, z_im;

            locked = QDP_D_expose_V(V->ptr);
            if (QDP_node_number(idx) == QDP_this_node) {
                QLA_D_Complex *zz = &QLA_D_elem_V(locked[QDP_index(idx)], c);
                z_re = QLA_real(*zz);
                z_im = QLA_imag(*zz);
            } else {
                z_re = 0;
                z_im = 0;
            }
            QDP_D_reset_V(V->ptr);
            QMP_sum_double(&z_re);
            QMP_sum_double(&z_im);
            QLA_real(*W) = z_re;
            QLA_imag(*W) = z_im;
        }
        qlua_free(L, idx);
        return 1;
    }
    case qString:
        return qlua_lookup(L, 2, opLatColVec);
    }
    return luaL_error(L, "bad index");
}

int
qLatColVec_put(lua_State *L)
{
    mLatColVec *V = qlua_checkLatColVec(L, 1);
    int c = qlua_checkcolorindex(L, 2, QDP_Nc);
    int *idx = qlua_latcoord(L, 2);

    if (idx == NULL) {
        mLatComplex *z = qlua_checkLatComplex(L, 3);
        
        QDP_D3_V_eq_elem_C(V->ptr, z->ptr, c, QDP_all);
    } else {
        QLA_D_Complex *z = qlua_checkComplex(L, 3);
        if (QDP_node_number(idx) == QDP_this_node) {
            QLA_D3_ColorVector *locked = QDP_D3_expose_V(V->ptr);
            QLA_D_Complex *zz = &QLA_D3_elem_V(locked[QDP_index(idx)], c);
            QLA_c_eq_c(*z, *zz);
            QDP_D3_reset_V(V->ptr);
        }
    }
    qlua_free(L, idx);
    return 0;
}

int
q_V_dot(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    mLatColVec *b = qlua_checkLatColVec(L, 2);
    mLatComplex *s = qlua_newLatComplex(L);

    QDP_D_C_eq_V_dot_V(s->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

int
q_V_gaussian(lua_State *L)
{
    mLatRandom *a = qlua_checkLatRandom(L, 1);
    mLatColVec *r = qlua_newLatColVec(L);

    QDP_D_V_eq_gaussian_S(r->ptr, a->ptr, QDP_all);

    return 1;
}


int
q_V_add_V(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    mLatColVec *b = qlua_checkLatColVec(L, 2);
    mLatColVec *c = qlua_newLatColVec(L);

    QDP_D_V_eq_V_plus_V(c->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

int
q_V_sub_V(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    mLatColVec *b = qlua_checkLatColVec(L, 2);
    mLatColVec *c = qlua_newLatColVec(L);

    QDP_D_V_eq_V_minus_V(c->ptr, a->ptr, b->ptr, QDP_all);

    return 1;
}

int
q_V_mul_r(lua_State *L)
{
    QLA_D_Real a = luaL_checknumber(L, 1);
    mLatColVec *b = qlua_checkLatColVec(L, 2);
    mLatColVec *c = qlua_newLatColVec(L);

    QDP_D_V_eq_r_times_V(c->ptr, &a, b->ptr, QDP_all);

    return 1;
}

int
q_r_mul_V(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    QLA_D_Real b = luaL_checknumber(L, 2);
    mLatColVec *c = qlua_newLatColVec(L);

    QDP_D_V_eq_r_times_V(c->ptr, &b, a->ptr, QDP_all);

    return 1;
}

int
q_V_mul_c(lua_State *L)
{
    QLA_D_Complex *a = qlua_checkComplex(L, 1);
    mLatColVec *b = qlua_checkLatColVec(L, 2);
    mLatColVec *c = qlua_newLatColVec(L);

    QDP_D_V_eq_c_times_V(c->ptr, a, b->ptr, QDP_all);

    return 1;
}

int
q_c_mul_V(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    QLA_D_Complex *b = qlua_checkComplex(L, 2);
    mLatColVec *c = qlua_newLatColVec(L);

    QDP_D_V_eq_c_times_V(c->ptr, b, a->ptr, QDP_all);

    return 1;
}

int
q_V_norm2(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    QLA_D_Real n;

    QDP_D_r_eq_norm2_V(&n, a->ptr, QDP_all);
    lua_pushnumber(L, n);
    
    return 1;
}

int
q_V_shift(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    QDP_Shift shift = qlua_checkShift(L, 2);
    QDP_ShiftDir dir = qlua_checkShiftDir(L, 3);
    mLatColVec *r = qlua_newLatColVec(L);

    QDP_D_V_eq_sV(r->ptr, a->ptr, shift, dir, QDP_all);

    return 1;
}

int
q_V_conj(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    mLatColVec *r = qlua_newLatColVec(L);

    QDP_D3_V_eq_conj_V(r->ptr, a->ptr, QDP_all);

    return 1;
}

int
q_neg_V(lua_State *L)
{
    mLatColVec *a = qlua_checkLatColVec(L, 1);
    mLatColVec *r = qlua_newLatColVec(L);
    QLA_D_Real m1 = -1;

    QDP_D3_V_eq_r_times_V(r->ptr, &m1, a->ptr, QDP_all);

    return 1;
}

int
q_latcolvec(lua_State *L)
{
    switch (lua_gettop(L)) {
    case 0: {
        mLatColVec *v = qlua_newLatColVec(L);

        QDP_D3_V_eq_zero(v->ptr, QDP_all);

        return 1;
    }
    case 1: {
        mLatColVec *a = qlua_checkLatColVec(L, 1);
        mLatColVec *r = qlua_newLatColVec(L);
        
        QDP_D3_V_eq_V(r->ptr, a->ptr, QDP_all);
        
        return 1;
    }
    case 2: {
        mLatComplex *c = qlua_checkLatComplex(L, 1);
        int a = luaL_checkinteger(L, 2);
        mLatColVec *r = qlua_newLatColVec(L);

        QDP_D3_V_eq_elem_C(r->ptr, c->ptr, a, QDP_all);

        return 1;
    }
    }
    return luaL_error(L, "bad arguments");
}

static struct luaL_Reg LatColVecMethods[] = {
    { "norm2",      q_V_norm2 },
    { "shift",      q_V_shift },
    { "conj",       q_V_conj },
    { NULL,         NULL }
};

static struct luaL_Reg mtLatColVec[] = {
    { "__tostring",        qLatColVec_fmt },
    { "__gc",              qLatColVec_gc },
    { "__index",           qLatColVec_get },
    { "__newindex",        qLatColVec_put },
    { "__umn",             q_neg_V },
    { "__add",             qlua_add },
    { "__sub",             qlua_sub },
    { "__mul",             qlua_mul },
    { "__div",             qlua_div },
    { NULL,                NULL }
};

static struct luaL_Reg fLatColVec[] = {
    { "ColorVector",       q_latcolvec },
    { NULL,                NULL }
};

int
init_latcolvec(lua_State *L)
{
    luaL_register(L, qcdlib, fLatColVec);
    qlua_metatable(L, mtnLatColVec, mtLatColVec);
    qlua_metatable(L, opLatColVec, LatColVecMethods);

    return 0;
}

int
fini_latcolvec(lua_State *L)
{
    return 0;
}