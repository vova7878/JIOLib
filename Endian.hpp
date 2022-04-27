#ifndef ENDIAN_HPP
#define ENDIAN_HPP

namespace JIO {

    template<typename T>
    class TypeArray {
    public:
        unsigned char bytes[sizeof (T)];
    };

    template<typename, bool le>
    class EInteger_Impl;

    template<typename T>
    class EInteger_Impl <T, false> : private TypeArray<T> {
    public:

        EInteger_Impl() {
            operator=(0);
        }

        EInteger_Impl(T value) {
            operator=(value);
        }

        EInteger_Impl(const EInteger_Impl<T, true> &other) {
            size_t offset = sizeof (T) - 1;
            for (unsigned i = 0; i < sizeof (T); i++) {
                this->bytes[i] = other.bytes[offset - i];
            }
        }

        operator const T() const {
            T value = 0;
            size_t offset = sizeof (T) - 1;
            for (int i = 0; i < sizeof (T); i++)
                value |= T(this->bytes[offset - i]) << (i << 3);
            return value;
        }

        const T operator=(const T value) {
            size_t offset = sizeof (T) - 1;
            for (int i = 0; i < sizeof (T); i++)
                this->bytes[offset - i] = value >> (i << 3);
            return value;
        }

        friend EInteger_Impl<T, true>;
    };

    template<typename T>
    class EInteger_Impl <T, true> : private TypeArray<T> {
    public:

        EInteger_Impl() {
            operator=(0);
        }

        EInteger_Impl(T value) {
            operator=(value);
        }

        EInteger_Impl(const EInteger_Impl<T, false> &other) {
            size_t offset = sizeof (T) - 1;
            for (unsigned i = 0; i < sizeof (T); i++) {
                this->bytes[i] = other.bytes[offset - i];
            }
        }

        operator const T() const {
            T value = 0;
            for (int i = 0; i < sizeof (T); i++)
                value |= T(this->bytes[i]) << (i << 3);
            return value;
        }

        const T operator=(const T value) {
            for (int i = 0; i < sizeof (T); i++)
                this->bytes[i] = value >> (i << 3);
            return value;
        }

        friend EInteger_Impl<T, false>;
    };

    template<typename T, bool le>
    class EInteger : public EInteger_Impl<T, le> {
    public:

        inline const T operator+=(const T t) {
            return *this = (*this +t);
        }

        inline const T operator-=(const T t) {
            return *this = (*this -t);
        }

        inline const T operator*=(const T t) {
            return *this = (*this * t);
        }

        inline const T operator/=(const T t) {
            return *this = (*this / t);
        }

        inline const T operator%=(const T t) {
            return *this = (*this % t);
        }

        inline const T operator|=(const T t) {
            return *this = (*this | t);
        }

        inline const T operator&=(const T t) {
            return *this = (*this & t);
        }

        inline const T operator^=(const T t) {
            return *this = (*this ^ t);
        }

        template<typename T2>
        inline const T operator>>=(const T2 t) {
            return *this = (*this >> t);
        }

        template<typename T2>
        inline const T operator<<=(const T2 t) {
            return *this = (*this << t);
        }

        inline EInteger<T, le> operator++(int) {
            EInteger tmp(*this);
            operator++();
            return tmp;
        }

        inline EInteger<T, le>& operator++() {
            T value = *this +1;
            *this = value;
            return *this;
        }

        inline EInteger<T, le> operator--(int) {
            EInteger tmp(*this);
            operator--();
            return tmp;
        }

        inline EInteger<T, le>& operator--() {
            T value = *this -1;
            *this = value;
            return *this;
        }

        using EInteger_Impl <T, le>::operator=;
    };
}

#endif /* ENDIAN_HPP */
