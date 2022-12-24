/// @file
/// @brief Файл с объявлением простого односвязного списка

#pragma once

#include <memory>


/// @brief Класс узла односвязного списка
/// @tparam T тип элемента списка
template<typename T>
struct Node
{
    /// @brief Конструктор
    /// @param val значение элемента
    /// @param next указатель на следующий узел
    Node(const T& val, Node* next) : val_(val), next_(next) { }

    /// @brief Получение указателя на элемент списка
    /// @return указатель на элемент списка
    T* valptr() noexcept
    {
        return &val_;
    }

    T val_;
    Node* next_ = nullptr;
};

/// @brief Класс простого односвязного списка
/// @tparam T тип элемента списка
/// @tparam Alloc класс аллокатора
template<typename T, typename Alloc = std::allocator<T>>
class SimpleFwdList
{
    using Node_ = Node<T>;
    using NodeAlloc_ = typename Alloc::template rebind<Node_>::other;
public:
    /// @brief Класс итератора по элементам простого односвязного списка
    struct Iterator : public std::iterator<std::forward_iterator_tag, Node_>
    {
        /// @brief Коструктор
        /// @param node указатель на узел списка
        Iterator(Node_* node) noexcept : node_(node) { }

        /// @brief Разименование итератора по *
        /// @return значение элемента списка
        T operator*() const noexcept
        {
            return *node_->valptr();
        }

        /// @brief Разименование итератора по ->
        /// @return указатель на элемент списка
        T* operator->() const noexcept
        {
            return node_->valptr();
        }

        /// @brief инкремент итератора
        /// @return итератор
        Iterator& operator++() noexcept
        {
            node_ = node_->next_;
            return *this;
        }

        /// @brief Сравнение итераторов на равенство
        /// @param x итератор
        /// @param y итератор
        /// @return одинаковые ли итераторы
        friend bool operator==(const Iterator& x, const Iterator& y) noexcept
        {
            return x.node_ == y.node_;
        }

        /// @brief Сравнение итераторов на неравенство
        /// @param x итератор
        /// @param y итератор
        /// @return одинаковые ли итераторы
        friend bool operator!=(const Iterator& x, const Iterator& y) noexcept
        {
            return x.node_ != y.node_;
        }

        Node_* node_;
    };

    using iterator = Iterator;

    /// @brief Конструктор
    SimpleFwdList()
    {
        before_ = new Node_(T(), head_);
    }

    /// @brief Деструктор
    ~SimpleFwdList()
    {
        Node_* node = head_;
        while (node)
        {
            Node_* next = node->next_;
            alloc_.destroy(node);
            alloc_.deallocate(node, 1);
            node = next;
        }
        delete before_;
    }

    /// @brief Получение итератора на начало списка
    /// @return итератор
    iterator begin() noexcept
    {
        return iterator(head_);
    }

    /// @brief Получение итератора на конец списка
    /// @return итератор
    iterator end() noexcept
    {
        return iterator(nullptr);
    }

    /// @brief Получение итератора на элемент, предшествующего началу списка
    /// @return итератор
    iterator before_begin() noexcept
    {
        return iterator(before_);
    }

    /// @brief Добавление элемента в начало списка
    /// @param val значение элемента
    void push_front(const T& val)
    {
        Node_* node = alloc_.allocate(1);
        alloc_.construct(node, val, head_);
        head_ = node;
    }

    /// @brief Добавление элемента в позицию за итератором
    /// @param pos итератор
    /// @param val значение элемента
    /// @return итератор на добавленный элемент
    iterator insert_after(iterator pos, const T& val)
    {
        Node_* node = alloc_.allocate(1);
        alloc_.construct(node, val, pos.node_->next_);
        pos.node_->next_ = node;

        if (pos == iterator(before_))
        {
            head_ = node;
        }
        return iterator(node);
    }

private:
    Node_* before_ = nullptr;
    Node_* head_ = nullptr;
    NodeAlloc_ alloc_;
};
