#include "api.h"
#include "api/math.h"
#include "math/math.h"
#include "lib/math.h"

static int l_lovrMat4Unpack(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  for (int i = 0; i < 16; i++) {
    lua_pushnumber(L, m[i]);
  }
  return 16;
}

int l_lovrMat4Set(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  for (int i = 0; i < 16; i++) {
    m[i] = luaL_checknumber(L, i);
  }
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4Copy(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  mat4 n = lovrPoolAllocate(lovrMathGetPool(), MATH_MAT4);
  mat4_init(n, m);
  luax_pushlightmathtype(L, n, MATH_MAT4);
  return 1;
}

static int l_lovrMat4Save(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  mat4 copy = lua_newuserdata(L, 16 * sizeof(float));
  mat4_init(copy, m);
  luaL_getmetatable(L, "mat4");
  lua_setmetatable(L, -2);
  return 1;
}

static int l_lovrMat4Identity(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  mat4_identity(m);
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4Inverse(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  mat4 n = lovrPoolAllocate(lovrMathGetPool(), MATH_MAT4);
  mat4_invert(mat4_init(n, m));
  luax_pushlightmathtype(L, n, MATH_MAT4);
  return 1;
}

static int l_lovrMat4Transpose(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  mat4 n = lovrPoolAllocate(lovrMathGetPool(), MATH_MAT4);
  mat4_transpose(mat4_init(n, m));
  luax_pushlightmathtype(L, n, MATH_MAT4);
  return 1;
}

static int l_lovrMat4Translate(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  if (lua_type(L, 2) == LUA_TNUMBER) {
    mat4_translate(m, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
  } else {
    float* v = luax_checkmathtype(L, 2, MATH_VEC3, "vec3 or number");
    mat4_translate(m, v[0], v[1], v[2]);
  }
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4Rotate(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  if (lua_type(L, 2) == LUA_TNUMBER) {
    mat4_rotate(m, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
  } else {
    float* q = luax_checkmathtype(L, 2, MATH_QUAT, "quat or number");
    mat4_rotateQuat(m, q);
  }
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4Scale(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  if (lua_type(L, 2) == LUA_TNUMBER) {
    float x = luaL_checknumber(L, 2);
    mat4_scale(m, x, luaL_optnumber(L, 3, x), luaL_optnumber(L, 4, x));
  } else {
    float* s = luax_checkmathtype(L, 2, MATH_VEC3, "vec3 or number");
    mat4_scale(m, s[0], s[1], s[2]);
  }
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4GetTransform(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  float x, y, z, sx, sy, sz, angle, ax, ay, az;
  mat4_getTransform(m, &x, &y, &z, &sx, &sy, &sz, &angle, &ax, &ay, &az);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  lua_pushnumber(L, z);
  lua_pushnumber(L, sx);
  lua_pushnumber(L, sy);
  lua_pushnumber(L, sz);
  lua_pushnumber(L, angle);
  lua_pushnumber(L, ax);
  lua_pushnumber(L, ay);
  lua_pushnumber(L, az);
  return 10;
}

static int l_lovrMat4SetTransform(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  float x = luaL_optnumber(L, 2, 0.);
  float y = luaL_optnumber(L, 3, 0.);
  float z = luaL_optnumber(L, 4, 0.);
  float sx = luaL_optnumber(L, 5, 1.);
  float sy = luaL_optnumber(L, 6, sx);
  float sz = luaL_optnumber(L, 7, sx);
  float angle = luaL_optnumber(L, 8, 0.);
  float ax = luaL_optnumber(L, 9, 0.);
  float ay = luaL_optnumber(L, 10, 1.);
  float az = luaL_optnumber(L, 11, 0.);
  mat4_setTransform(m, x, y, z, sx, sy, sz, angle, ax, ay, az);
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4Perspective(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  float clipNear = luaL_checknumber(L, 2);
  float clipFar = luaL_checknumber(L, 3);
  float fov = luaL_checknumber(L, 4);
  float aspect = luaL_checknumber(L, 5);
  mat4_perspective(m, clipNear, clipFar, fov, aspect);
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4Orthographic(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  float left = luaL_checknumber(L, 2);
  float right = luaL_checknumber(L, 3);
  float top = luaL_checknumber(L, 4);
  float bottom = luaL_checknumber(L, 5);
  float clipNear = luaL_checknumber(L, 6);
  float clipFar = luaL_checknumber(L, 7);
  mat4_orthographic(m, left, right, top, bottom, clipNear, clipFar);
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4LookAt(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  float from[3] = { luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4) };
  float to[3] = { luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7) };
  float up[3] = { luaL_optnumber(L, 8, 0), luaL_optnumber(L, 9, 1), luaL_optnumber(L, 10, 0) };
  mat4_lookAt(m, from, to, up);
  lua_settop(L, 1);
  return 1;
}

static int l_lovrMat4__mul(lua_State* L) {
  mat4 m = luax_checkmathtype(L, 1, MATH_MAT4, NULL);
  mat4 n = luax_checkmathtype(L, 2, MATH_MAT4, NULL);
  mat4 out = lovrPoolAllocate(lovrMathGetPool(), MATH_MAT4);
  mat4_multiply(mat4_init(out, m), n);
  luax_pushlightmathtype(L, out, MATH_MAT4);
  return 1;
}

const luaL_Reg lovrMat4[] = {
  { "unpack", l_lovrMat4Unpack },
  { "set", l_lovrMat4Set },
  { "copy", l_lovrMat4Copy },
  { "save", l_lovrMat4Save },
  { "identity", l_lovrMat4Identity },
  { "inverse", l_lovrMat4Inverse },
  { "transpose", l_lovrMat4Transpose },
  { "translate", l_lovrMat4Translate },
  { "rotate", l_lovrMat4Rotate },
  { "scale", l_lovrMat4Scale },
  { "getTransform", l_lovrMat4GetTransform },
  { "setTransform", l_lovrMat4SetTransform },
  { "perspective", l_lovrMat4Perspective },
  { "orthographic", l_lovrMat4Orthographic },
  { "lookAt", l_lovrMat4LookAt },
  { "__mul", l_lovrMat4__mul },
  { NULL, NULL }
};