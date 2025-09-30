#pragma once
#include <concepts>


template<typename T>
concept MouseAPI = requires(T m) {
	{ m.GetX() } ->std::same_as<void>;
};