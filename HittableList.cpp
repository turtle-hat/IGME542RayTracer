#include "HittableList.h"

HittableList::HittableList(std::shared_ptr<Hittable> _object)
{
	Add(_object);
}

void HittableList::Clear()
{
	objects.clear();
}

void HittableList::Add(std::shared_ptr<Hittable> _object)
{
	objects.push_back(_object);
}

bool HittableList::Hit(const Ray& _ray, float _rayTMin, float _rayTMax, HitRecord& _record) const
{
	HitRecord temporaryRecord;
	bool hasHitAnything = false;
	float closestSoFar = _rayTMax;

	for (const auto& object : objects) {
		if (object->Hit(_ray, _rayTMin, closestSoFar, temporaryRecord)) {
			hasHitAnything = true;
			closestSoFar = temporaryRecord.t;
			_record = temporaryRecord;
		}
	}

	return hasHitAnything;
}
