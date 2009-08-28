#ifndef MARK_3DF9923F_966E_47B3_ABAA_79CFF3A58E8C
#define MARK_3DF9923F_966E_47B3_ABAA_79CFF3A58E8C

typedef struct {
    QDP_RandomState *ptr;
} mLatRandom;

extern const char *mtnLatRandom;

mLatRandom *qlua_checkLatRandom(lua_State *L, int idx);
mLatRandom *qlua_newLatRandom(lua_State *L);

int init_latrandom(lua_State *L);
int fini_latrandom(lua_State *L);

#endif /* !defined(MARK_3DF9923F_966E_47B3_ABAA_79CFF3A58E8C) */
