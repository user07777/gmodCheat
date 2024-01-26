#pragma once
#include <math.h>
#include "Vector3.h"
#include "esp.h"
float GetDist(Vec3 myPos,Vec3 Dt) {
    Vec3 D = Vec3{
        Dt.x - myPos.x,
        Dt.y - myPos.y,
        Dt.z - myPos.z,

    };
    float dist = sqrt(pow(Dt.x - myPos.x, 2) + pow(Dt.y - myPos.y, 2) + pow(Dt.z - myPos.z, 2));

    return  dist;
}
Vec3 calcAngle(Vec3 lplayer, Vec3 ent) {
    {
        lplayer.z += 64.0f;
        Vec3 Difference;
        Difference = ent - lplayer;


        float magnitude = sqrt(pow(Difference.x, 2) + pow(Difference.y, 2) + pow(Difference.z, 2));

        Vec3 AngleToAim;

        AngleToAim.y = atan2f(Difference.y, Difference.x) * 180 / 3.14;

        AngleToAim.x = -asinf(Difference.z / magnitude) * 180 / 3.14;


        if (AngleToAim.x > 89)
            AngleToAim.x = 89;

        if (AngleToAim.x < -89)
            AngleToAim.x = -89;

        while (AngleToAim.y > 180) { AngleToAim.y -= 360; }

        while (AngleToAim.y < -180) { AngleToAim.y += 360; }


        AngleToAim.z = 0.0f;



        return AngleToAim;
    }
}

void ClampAngle(Vec3& angle) {
    if (angle.x > 89.0f) angle.x = 89.f;
    if (angle.x < -89.0f) angle.x = -89.f;

    if (angle.y > 180.f) angle.y = 180.f;
    if (angle.y < -180.f) angle.y = -180.f;

    angle.z = 0.0f;
}