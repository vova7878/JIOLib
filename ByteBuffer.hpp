#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include <cstring>
#include <memory>
#include "checks.hpp"
#include "exceptions.hpp"

namespace JIO {

    template<bool>
    class ByteBuffer;

    inline void free_deleter(void *ptr) {
        free(ptr);
    }

    inline void null_deleter(void*) { }

    template<>
    class ByteBuffer<false> {
    private:
        size_t start;
        size_t _capacity;
        std::shared_ptr<char> ptr_data;

        inline ByteBuffer(const std::shared_ptr<char> &ptr, size_t start,
                size_t capacity) :
        start(start), _capacity(capacity), ptr_data(ptr) { }
    public:

        inline ByteBuffer(void *ptr, size_t start, size_t capacity, bool clean) :
        start(0), _capacity(capacity),
        ptr_data(checkUBounds<char*>(ptr, start, capacity),
        clean ? free_deleter : null_deleter) { }

        inline ByteBuffer(void *ptr, size_t start, size_t capacity) :
        ByteBuffer(ptr, start, capacity, true) { }

        inline ByteBuffer(size_t capacity) :
        ByteBuffer(new char[capacity], 0, capacity) { }

        inline void set(ByteBuffer other) {
            ptr_data = other.ptr_data;
            _capacity = other._capacity;
            start = other.start;
        }

        inline size_t capacity() const {
            return _capacity;
        }

        inline char* getBase() const {
            return ptr_data.get();
        }

        inline char* getData() const {
            return ptr_data.get() + start;
        }

        inline void get(size_t index, void *dst, size_t length) const {
            checkRange(index, length, _capacity);
            std::memmove(dst, getData() + index, length);
        }

        inline void put(size_t index, const void *src, size_t length) {
            checkRange(index, length, _capacity);
            std::memmove(getData() + index, src, length);
        }

        inline void get(size_t index, ByteBuffer dst,
                size_t offset, size_t length) const {
            checkRange(index, length, _capacity);
            checkRange(offset, length, dst._capacity);
            std::memmove(dst.getData() + offset,
                    getData() + index, length);
        }

        inline void put(size_t index, const ByteBuffer src,
                size_t offset, size_t length) {
            checkRange(index, length, _capacity);
            checkRange(offset, length, src._capacity);
            std::memmove(getData() + index,
                    src.getData() + offset, length);
        }

        template<typename T>
        inline void getObject(size_t index, T &obj) const {
            get(index, &obj, sizeof (T));
        }

        template<typename T>
        inline void putObject(size_t index, const T &obj) {
            put(index, &obj, sizeof (T));
        }

        inline void copy(size_t fromIndex, size_t toIndex, size_t length) {
            checkRange(fromIndex, length, _capacity);
            checkRange(toIndex, length, _capacity);
            char *data = getData();
            std::memmove(data + toIndex, data + fromIndex, length);
        }

        inline const ByteBuffer slice(size_t index, size_t length) const {
            checkRange(index, length, _capacity);
            return ByteBuffer(ptr_data, start + index, length);
        }

        inline ByteBuffer slice(size_t index, size_t length) {
            checkRange(index, length, _capacity);
            return ByteBuffer(ptr_data, start + index, length);
        }

        inline ByteBuffer clone(size_t index, size_t length) const {
            checkRange(index, length, _capacity);
            ByteBuffer out(length);
            get(index, out, 0, length);
            return out;
        }

        inline ByteBuffer clone() const {
            return clone(0, _capacity);
        }

        ByteBuffer<true> operator[](size_t);
        const ByteBuffer<true> operator[](size_t) const;

        friend ByteBuffer<true>;
    };

    template<>
    class ByteBuffer <true> : public ByteBuffer<false> {
    private:
        mutable size_t _position;

        inline ByteBuffer(const std::shared_ptr<char> &ptr, size_t start,
                size_t capacity) :
        ByteBuffer<false>(ptr, start, capacity), _position(0) { }
    public:

        inline ByteBuffer(void *ptr, size_t start, size_t capacity, bool clean) :
        ByteBuffer<false>(ptr, start, capacity, clean), _position(0) { }

        inline ByteBuffer(void *ptr, size_t start, size_t capacity) :
        ByteBuffer<false>(ptr, start, capacity), _position(0) { }

        inline ByteBuffer(size_t capacity) : ByteBuffer<false>(capacity),
        _position(0) { }

        inline void set(ByteBuffer<true> other) {
            ptr_data = other.ptr_data;
            _capacity = other._capacity;
            start = other.start;
            _position = other._position;
        }

        using ByteBuffer<false>::get;
        using ByteBuffer<false>::put;
        using ByteBuffer<false>::getObject;
        using ByteBuffer<false>::putObject;

        inline void get(void *dst, size_t length) const {
            size_t tmp = _position;
            get(tmp, dst, length);
            _position = tmp + length;
        }

        inline void put(const void *src, size_t length) {
            size_t tmp = _position;
            put(tmp, src, length);
            _position = tmp + length;
        }

        template<typename T>
        inline void getObject(T &obj) const {
            get(&obj, sizeof (T));
        }

        template<typename T>
        inline void putObject(const T &obj) {
            put(&obj, sizeof (T));
        }

        const inline ByteBuffer<true> slice(size_t index, size_t length) const {
            checkRange(index, length, _capacity);
            return ByteBuffer<true>(ptr_data, start + index, length);
        }

        inline ByteBuffer<true> slice(size_t index, size_t length) {
            checkRange(index, length, _capacity);
            return ByteBuffer<true>(ptr_data, start + index, length);
        }

        inline ByteBuffer<true> clone(size_t index, size_t length) const {
            checkRange(index, length, _capacity);
            ByteBuffer<true> out(length);
            get(index, out, 0, length);
            return out;
        }

        inline ByteBuffer<true> clone() const {
            return clone(0, _capacity);
        }

        inline size_t position() const {
            return _position;
        }

        inline void position(size_t newPosition) const {
            if (newPosition > _capacity) {
                throw IllegalArgumentException(
                        "newPosition > capacity: (",
                        newPosition, " > ", _capacity, ")");
            }
            _position = newPosition;
        }

        template<typename T>
        ByteBuffer<true>& operator<<(T &obj) {
            putObject(obj);
            return *this;
        }

        template<typename T>
        const ByteBuffer<true>& operator>>(T &obj) const {
            getObject(obj);
            return *this;
        }

        template<typename T>
        ByteBuffer<true>& operator>>(T &obj) {
            getObject(obj);
            return *this;
        }

        friend ByteBuffer<false>;
    };

    inline ByteBuffer<true> ByteBuffer<false>::operator[](size_t pos) {
        ByteBuffer<true> out(ptr_data, start, _capacity);
        out.position(pos);
        return out;
    }

    inline const ByteBuffer<true>
    ByteBuffer<false>::operator[](size_t pos) const {
        ByteBuffer<true> out(ptr_data, start, _capacity);
        out.position(pos);
        return out;
    }
}

#endif /* BYTEBUFFER_HPP */

