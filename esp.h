#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include "Vector3.h"
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vec4 { float x, y, z, w; };
class ent {
public:
	union {
		DEFINE_MEMBER_N(bool, inativo, 0x7E);
		DEFINE_MEMBER_N(int, health, 0x90);
		DEFINE_MEMBER_N(int, team, 0x9c);
		DEFINE_MEMBER_N(Vec3, m_vecOrigin, 0x260);
		DEFINE_MEMBER_N(Vec3, v1, 0x264);
		DEFINE_MEMBER_N(Vec3, v2, 0x2CC);

		DEFINE_MEMBER_N(int, boneArr, 0x13F0);
	};
};

class EntList_ {
public:
	ent* ent;
	char pad[12];
};

class Entlist {
public:
	EntList_ entlist[64];
};

class Esp
{
public:
	uintptr_t engine = (uintptr_t)GetModuleHandle(L"engine.dll");
	uintptr_t client = (uintptr_t)GetModuleHandle(L"client.dll");
	Entlist* entlist = (Entlist*)(client + 0x0756B04);
	ent* localPlayer = entlist->entlist[0].ent;
	ID3DXLine* m_Line;
	uintptr_t viewMatrixaddr = *(uintptr_t*)(engine + 0x0121CAC);
	float viewMatrix[16];
	ID3DXFont* font;
	void update();
	bool isValid(ent* ent);
	bool Wrld2Screen(Vec3 pos, Vec3& screen,int width,int height);
	Vec3 bonePos(ent*, int bone);
	Vec3* vecview = (Vec3*)((uintptr_t)(GetModuleHandle(L"engine.dll")) + 0x504854);

};


