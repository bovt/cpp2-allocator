/// @file
/// @brief Файл с объявлением и реализацией аллокатора, выделяющего фиксированный размер памяти единоразово

#pragma once

#include <cassert>
#include <algorithm>
#include <array>

/// @brief Класс аллокатора, выделяющего фиксированный размер памяти единоразово
/// @tparam T тип объектов
/// @tparam Size размер
template<typename T, int Size>
struct FixedAllocator
{
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    /// @brief Вспомогательная структура
    /// @tparam U тип
    template<typename U>
    struct rebind
    {
        using other = FixedAllocator<U, Size>;
    };

    /// @brief Конструктор
    FixedAllocator() = default;

    /// @brief Деструктор
    ~FixedAllocator() = default;

    /// @brief Выделение памяти
    /// @param n количество элементов
    /// @return указатель на выделенную память
    T* allocate(std::size_t n)
    {
        if (n == 0)
        {
            return nullptr;
        }
        else if ((poolSize_ + n) > Size)
        {
            throw std::bad_alloc();
        }

        T* p = nullptr;
        if (poolSize_ == 0) // Сценарий выделения новой памяти
        {
            ptr_ = static_cast<T*>( std::malloc(sizeof(T) * Size) );
            if (!ptr_)
            {
                throw std::bad_alloc();
            }
            // Фиксация используемых адресов
            for (; poolSize_ < n; ++poolSize_)
            {
                ptrPool_[poolSize_] = ptr_ + poolSize_;
            }
            p = ptr_;
        }
        else // Сценарий добавления элемента в выделенную память
        {
            auto it = std::find(ptrPool_.begin(), ptrPool_.end(), nullptr);
            assert(it != ptrPool_.end()); // должны быть сбободные адреса, т.к. есть проверка на размер в начале

            // Если n > 1, то элементы обязаны быть расположены в памяти последовательно
            int index = 0;
            while (it != ptrPool_.end())
            {
                index = std::distance(ptrPool_.begin(), it);
                // Поиск непрерывной последовательности для всех n
                std::size_t found = 1;
                for (; found < n; ++found)
                {
                    if (ptrPool_[index + found] != nullptr)
                    {
                        it = std::find(it+1, ptrPool_.end(), nullptr);
                        break;
                    }
                }
                if (found == n)
                {
                    break;
                }
            }
            if (it == ptrPool_.end()) // нет последовательных адресов
            {
                throw std::bad_alloc();
            }

            p = ptr_ + index;
            // Фиксация используемых адресов
            for (decltype(n) i = 0; i < n; ++i)
            {
                ptrPool_[index + i] = ptr_ + index + i;
                ++poolSize_;
            }
        }
        return p;
    }

    /// @brief Освобождение памяти
    /// @param p указатель на память
    /// @param n количество элементов
    void deallocate(T* p, std::size_t n)
    {
        auto it = std::find(ptrPool_.begin(), ptrPool_.end(), p);
        assert(it != ptrPool_.end());

        // Фиксация освобожденных адресов
        for (decltype(n) i = 0; i < n; ++i)
        {
            *it = nullptr;
            --poolSize_;
            ++it;
        }
        if (poolSize_ == 0)
        {
            std::free(ptr_);
            ptr_ = nullptr;
        }
    }

    /// @brief Конструирование объекта
    /// @tparam U тип объекта
    /// @tparam ...Args типы параметров конструктора объекта
    /// @param p указатель на объект
    /// @param ...args параметры конструктора объекта
    template<typename U, typename ...Args>
    void construct(U* p, Args &&...args)
    {
        new(p) U(std::forward<Args>(args)...);
    }

    /// @brief Уничтожение объекта
    /// @param p указатель на объект
    void destroy(T* p)
    {
        p->~T();
    }

    /// @brief Получение адреса памяти
    /// @return адрес на начало памяти
    const T* data() const
    {
        return ptr_;
    }

    /// @brief Получение размера используемой памяти
    /// @return размер используемой памяти
    std::size_t size() const
    {
        return poolSize_;
    }
private:
    T* ptr_ = nullptr;
    std::array<T*, Size> ptrPool_ = {}; // инициализируется нулями
    std::size_t poolSize_ = 0;
};
