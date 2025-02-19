#pragma once
#include "Hittable.h"

class Sphere :
    public Hittable
{
public:
    Sphere(DirectX::XMFLOAT3 _origin, float _radius) : origin(_origin), radius(fmax(0, _radius)) {}
    bool Hit(const Ray& _ray, float _rayTMin, float _rayTMax, HitRecord& _record) const override;
private:
    DirectX::XMFLOAT3 origin;
    float radius;
};

