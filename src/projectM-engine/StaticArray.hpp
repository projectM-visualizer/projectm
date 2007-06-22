#ifndef PROJECTM_STATIC_ARRAY_HPP
#define PROJECTM_STATIC_ARRAY_HPP
#include <cassert>
/**
* A simple array implementation with a few important features:
 * (1) static array length, resulting in compile time optimizations
 * (2) bounds checking when compiling with assertions
 * Note that this data structure is generally useful only for smaller sized arrays.
*/

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
class StaticArray {
public:

    typedef T *  iterator;
    typedef  const T * const_iterator;

    StaticArray();
    StaticArray(const std::size_t logical_size);
    StaticArray(const std::size_t logical_size, const T & defaultValue);
    const T & operator[] (std::size_t index) const;
    T & operator[] (std::size_t index);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    void clear();
    bool empty() const;

    bool operator==(const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const;
    bool operator!=(const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const;
    bool operator<(const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const;
    bool operator>(const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const;

    /// Do nothing implementation that will be optimized out. Bit of a hack to make it interface with vector.
    void reserve(const std::size_t amount) const {}

    void push_back(const T & element);
    void pop_back();

    void erase(iterator first, iterator last);

    std::size_t size() const;
    std::size_t capacity() const;

private:
    std::size_t m_logical_size;
    T m_array[STATIC_ARRAY_MAX_SIZE+1];
};


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
StaticArray<T, STATIC_ARRAY_MAX_SIZE>::StaticArray():m_logical_size(0) {}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
StaticArray<T, STATIC_ARRAY_MAX_SIZE>::StaticArray(const std::size_t logical_size):m_logical_size(logical_size) {
    assert(logical_size <= STATIC_ARRAY_MAX_SIZE);
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
StaticArray<T, STATIC_ARRAY_MAX_SIZE>::StaticArray(const std::size_t logical_size, const T & defaultValue):m_logical_size(logical_size) {
    assert(logical_size <= STATIC_ARRAY_MAX_SIZE);
    for (iterator pos = begin(); pos != end();++pos)
        *pos = defaultValue;
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline const T & StaticArray<T, STATIC_ARRAY_MAX_SIZE>::operator[] (std::size_t index) const {
    assert(index <= (m_logical_size-1));
    return m_array[index];
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline T & StaticArray<T, STATIC_ARRAY_MAX_SIZE>::operator[] (std::size_t index) {
    assert(index <= (m_logical_size-1));
    return m_array[index];
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline std::size_t StaticArray<T, STATIC_ARRAY_MAX_SIZE>::size() const {
    return m_logical_size;
}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline std::size_t StaticArray<T, STATIC_ARRAY_MAX_SIZE>::capacity() const {
    return STATIC_ARRAY_MAX_SIZE;
}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline bool StaticArray<T, STATIC_ARRAY_MAX_SIZE>::empty() const {
    return m_logical_size == 0;
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline void StaticArray<T, STATIC_ARRAY_MAX_SIZE>::clear() {
    m_logical_size = 0;
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline typename StaticArray<T, STATIC_ARRAY_MAX_SIZE>::const_iterator StaticArray<T, STATIC_ARRAY_MAX_SIZE>::begin() const {
    return m_array;
}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline typename StaticArray<T, STATIC_ARRAY_MAX_SIZE>::iterator StaticArray<T, STATIC_ARRAY_MAX_SIZE>::begin() {
    return m_array;
}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline typename StaticArray<T, STATIC_ARRAY_MAX_SIZE>::const_iterator StaticArray<T, STATIC_ARRAY_MAX_SIZE>::end() const {
    return m_array+m_logical_size;
}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline void StaticArray<T, STATIC_ARRAY_MAX_SIZE>::push_back(const T & element)  {
    (*this)[m_logical_size++] = element;
}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline void StaticArray<T, STATIC_ARRAY_MAX_SIZE>::pop_back()  {
    assert(m_logical_size > 0);
    m_logical_size--;
}


/// @slow worst case is around N^2 + N.
template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline void StaticArray<T, STATIC_ARRAY_MAX_SIZE>::erase(iterator first, iterator last)  {

    StaticArray<T,STATIC_ARRAY_MAX_SIZE> tmpArray;

    for (iterator pos = begin(); pos != end();++pos) {
        for (iterator spos = first; spos != last;++spos) {
            if (pos != spos)
                tmpArray.push_back(*pos);
        }
    }
    (*this) =tmpArray;
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline typename StaticArray<T, STATIC_ARRAY_MAX_SIZE>::iterator StaticArray<T, STATIC_ARRAY_MAX_SIZE>::end() {
    return m_array+m_logical_size;
}


/// Lexographic comparison
template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline bool StaticArray<T, STATIC_ARRAY_MAX_SIZE>::operator< (const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const {

    const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & lhs = *this;

    std::size_t len = rhs.size() < lhs.size() ? rhs.size(): lhs.size();

    for (std::size_t i = 0; i < len;i++) {
        if (lhs[i] < rhs[i])
            return true;
        else if (!(lhs[i] == rhs[i]))
            return false;
    }

    // rhs has less elements than lhs
    if (len < lhs.size())
        return false;

    // lhs has less elements than rhs
    if (len < rhs.size())
        return true;

    // Equal
    return false;

}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline bool StaticArray<T, STATIC_ARRAY_MAX_SIZE>::operator!=(const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const {
    return !(*this == rhs);
}

template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline bool StaticArray<T, STATIC_ARRAY_MAX_SIZE>::operator==(const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const {

    const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & lhs = *this;

    if (rhs.size() != lhs.size())
        return false;

    for (std::size_t i = 0; i < rhs.size() ;i++) {
        if (!(rhs[i] == lhs[i]))
            return false;
    }
    return true;

}


template <class T, std::size_t STATIC_ARRAY_MAX_SIZE>
inline bool StaticArray<T, STATIC_ARRAY_MAX_SIZE> ::operator> (const StaticArray<T, STATIC_ARRAY_MAX_SIZE> & rhs) const {
	return ((!((*this) == rhs)) && (!((*this) < rhs)));
}



#endif