#pragma once

#include <cmath>
#include <SFML/System/Vector2.hpp>

using namespace sf;

template<typename T>
float distance(Vector2<T> a, Vector2<T> b) {
	Vector2<T> c = a - b;
	return std::sqrt(c.x*c.x + c.y*c.y);
}

template<typename T>
float norm(Vector2<T> v) {
	return distance<T>(v, Vector2<T>());
}

float clamp(float x, float a, float b) {
	if (x < a) {
		return a;
	} else if (x > b) {
		return b;
	}

	return x;
}

float lerp(float a, float b, float t) {
	return t*b + (1-t)*a;
}

// x is mapped from [a, b] to [c, d] maintaining proportions
float map(float x, float a, float b, float c, float d) {
	return (x - a)*(d - c)/(b - a) + c;
}
