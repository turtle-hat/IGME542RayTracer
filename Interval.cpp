#include "Interval.h"

float Interval::Size() const
{
    return maximum - minimum;
}

bool Interval::Contains(float _x) const
{
    return minimum <= _x && _x <= maximum;
}

bool Interval::Surrounds(float _x) const
{
    return minimum < _x && _x < maximum;
}

float Interval::Clamp(float _x) const
{
    if (_x < minimum) return minimum;
    if (_x > maximum) return maximum;
    return _x;
}

const Interval Interval::Empty      = Interval(+infinity, -infinity);
const Interval Interval::Universe   = Interval(-infinity, +infinity);
