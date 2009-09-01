#include <qlua.h>
#include <lattice.h>
#include <latint.h>

int qRank = 0;
int *qDim = NULL;

int *
qlua_latcoord(lua_State *L, int n)
{
    int d, i;
    int *idx;

    luaL_checktype(L, n, LUA_TTABLE);
    d = lua_objlen(L, n);
    if (d != qRank) {
        return NULL;
    }
    idx = qlua_malloc(L, d * sizeof (int));
    for (i = 0; i < d; i++) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, n);
        idx[i] = luaL_checkint(L, -1);
        if ((idx[i] < 0) || (idx[i] >= qDim[i])) {
            qlua_free(L, idx);
            return NULL;
        }
    }
    
    return idx;
}

int *
qlua_checklatcoord(lua_State *L, int n)
{
    int *idx = qlua_latcoord(L, n);

    if (idx == 0)
        luaL_error(L, "bad lattice coordinates");

    return idx;
}

static int pcoord_d = -1; /* YYY global state */
static void
pcoord_set(QLA_Int *dst, int coords[])
{
    *dst = coords[pcoord_d];
}

static int
q_pcoord(lua_State *L)
{
    int d = luaL_checkint(L, 1);
    mLatInt *v = qlua_newLatInt(L);
    
    if ((d < 0) || (d >= qRank))
        return luaL_error(L, "coordinate out of range");
    
    /* YYY global state */
    pcoord_d = d;
    QDP_I_eq_func(v->ptr, pcoord_set, QDP_all);

    return 1;
}

static int
q_lattice(lua_State *L)
{
    int r, i;

    if (qRank != 0)
        return luaL_error(L, "redefining the lattice");

    luaL_checktype(L, 1, LUA_TTABLE);
    r = lua_objlen(L, 1);
    if (r <= 0)
        return luaL_error(L, "Bad lattice rank");
    qRank = r;
    qDim = qlua_malloc(L, r * sizeof (int));
    for (i = 0; i < r; i++) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, 1);
        qDim[i] = luaL_checkint(L, -1);
    }
    QDP_set_latsize(qRank, qDim);
    if (QDP_create_layout()) {
        return luaL_error(L, "can not create lattice");
    }
    
    return 0;
}

static int
q_dims(lua_State *L)
{
    int i;

    lua_createtable(L, qRank, 0);
    for (i = 0; i < qRank; i++) {
        lua_pushnumber(L, qDim[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static struct luaL_Reg fLattice[] = {
    { "lattice", q_lattice },
    { "pcoord",  q_pcoord },
    { "dims",    q_dims },
    { NULL, NULL}
};

int
init_lattice(lua_State *L)
{
    luaL_register(L, qcdlib, fLattice);

    return 0;
}

int
fini_lattice(lua_State *L)
{
    qlua_free(L, qDim);
    qDim = 0;
    qRank = 0;

    return 0;
}
