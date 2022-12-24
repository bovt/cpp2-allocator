/// @file
/// @brief Файл с функцией вычисления факториала

#pragma once

#include <cstddef>

/// @brief Функция вычисления факториала
/// @param n число
/// @return факториал n
inline std::size_t factorial(int n) noexcept
{
    std::size_t f = 1;
    for (int i = 1; i <= n; ++i)
    {
        f *= i;
    }
    return f;
}
