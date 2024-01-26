#include "esp.h"
#include <iostream>
#include "Esp.h"
#include "draw.h"

CDraw ddraw;
void Esp::update() {
	memcpy(&viewMatrix, (BYTE*)(viewMatrixaddr), sizeof(viewMatrix));
	vecview = (Vec3*)((uintptr_t)(GetModuleHandle(L"engine.dll")) + 0x504854);

}
bool Esp::isValid(ent* entity) {
	if ((entity == nullptr) ||
		(entity == localPlayer) ||
		(entity->health) <= 0) 
		 {
		return false;
	}
	else {
		return true;
	}
}


bool Esp::Wrld2Screen(Vec3 pos, Vec3& screen,int width,int height){
	Vec4 clip;
	//multiplicando as coordenadas do mundo (olho) pela matriz de proje��o
	//Interando por 1 , caso cs go usasse openGl seria por 4
	clip.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clip.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clip.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clip.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clip.w < 0.1f)
		return false;
	//dividindo cada elemento da matriz por w = temos as coordenadas normalizadas   
	Vec3 NDC;
	NDC.x = clip.x / clip.w;
	NDC.y = clip.y / clip.w;
	NDC.z = clip.z / clip.w;

	//----------------------------------
	// get windowWidth,windowheight
	//-----------------------------------------------

	screen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
	screen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);
	return true;
}

Vec3 Esp::bonePos(ent* entity, int id) {
	uintptr_t bone = entity->boneArr;
	Vec3 bonePos{};

	bonePos.x = *(float*)(bone + 0x30 * id + 0x0C);
	bonePos.y = *(float*)(bone + 0x30 * id + 0x1C);
	bonePos.z = *(float*)(bone + 0x30 * id + 0x2C);
	return bonePos;
}