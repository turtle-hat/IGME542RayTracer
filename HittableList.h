#pragma once
#include "Hittable.h"

#include <vector>
#include "Helpers.h"

class HittableList :
    public Hittable
{
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> _object);

    void Clear();
    void Add(shared_ptr<Hittable> _object);
    bool Hit(const Ray& _ray, float _rayTMin, float _rayTMax, HitRecord& _record) const override;
};

