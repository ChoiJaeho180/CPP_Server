#pragma once
#include <random>

class MathUtils
{
public:
	template<typename T>
	static T rand(T min, T max) {
		std::random_device randomDevice;
		std::mt19937 generator(randomDevice());

		if constexpr (std::is_integral_v<T>) {
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(generator);
		}
		else {
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(generator);
		}
	}
};

