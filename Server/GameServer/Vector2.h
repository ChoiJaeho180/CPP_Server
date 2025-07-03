#pragma once
template<typename T>
struct Vector2
{
	T x;
	T y;

	Vector2() = default;

	Vector2(T x_, T y_)
		: x(x_), y(y_) {
	}
};

