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

    template<typename I>
    class EInteger_Impl <I, false> : private TypeArray<I> {
    public:

        inline EInteger_Impl() {
            operator=(0);
        }

        inline EInteger_Impl(I value) {
            operator=(value);
        }

        inline EInteger_Impl(const EInteger_Impl<I, true> &other) {
            size_t offset = sizeof (I) - 1;
            for (unsigned i = 0; i < sizeof (I); i++) {
                this->bytes[i] = other.bytes[offset - i];
            }
        }

        inline operator const I() const {
            I value = 0;
            size_t offset = sizeof (I) - 1;
            for (int i = 0; i < sizeof (I); i++) {
                value |= I(this->bytes[offset - i]) << (i << 3);
            }
            return value;
        }

        inline const I operator=(const I value) {
            size_t offset = sizeof (I) - 1;
            for (int i = 0; i < sizeof (I); i++) {
                this->bytes[offset - i] = value >> (i << 3);
            }
            return value;
        }

        friend EInteger_Impl<I, true>;
    };

    template<typename I>
    class EInteger_Impl <I, true> : private TypeArray<I> {
    public:

        inline EInteger_Impl() {
            operator=(0);
        }

        inline EInteger_Impl(I value) {
            operator=(value);
        }

        inline EInteger_Impl(const EInteger_Impl<I, false> &other) {
            size_t offset = sizeof (I) - 1;
            for (unsigned i = 0; i < sizeof (I); i++) {
                this->bytes[i] = other.bytes[offset - i];
            }
        }

        inline operator const I() const {
            I value = 0;
            for (int i = 0; i < sizeof (I); i++) {
                value |= I(this->bytes[i]) << (i << 3);
            }
            return value;
        }

        inline const I operator=(const I value) {
            for (int i = 0; i < sizeof (I); i++) {
                this->bytes[i] = value >> (i << 3);
            }
            return value;
        }

        friend EInteger_Impl<I, false>;
    };

    template<size_t len, typename T1, typename T2>
    inline void _cast(const T1 &from, T2 &to) {
        const char *data1 = reinterpret_cast<const char*> (&from);
        char *data2 = reinterpret_cast<char*> (&to);
        for (size_t i = 0; i < len; i++) {
            data2[i] = data1[i];
        }
    }

    template<typename, typename, bool le>
    class EFloat_Impl;

    template<typename F, typename I>
    class EFloat_Impl <F, I, false> : private TypeArray<F> {
        static_assert(sizeof (I) == sizeof (F), "Size of I is not equal size of F");
    public:

        inline EFloat_Impl() {
            operator=(0);
        }

        inline EFloat_Impl(F value) {
            operator=(value);
        }

        inline EFloat_Impl(const EFloat_Impl<F, I, true> &other) {
            size_t offset = sizeof (F) - 1;
            for (unsigned i = 0; i < sizeof (F); i++) {
                this->bytes[i] = other.bytes[offset - i];
            }
        }

        inline operator const F() const {
            I tmp = 0;
            F out;
            size_t offset = sizeof (F) - 1;
            for (int i = 0; i < sizeof (F); i++) {
                tmp |= F(this->bytes[offset - i]) << (i << 3);
            }
            _cast<sizeof (F)>(tmp, out);
            return out;
        }

        inline const F operator=(const F value) {
            I tmp = 0;
            _cast<sizeof (F)>(value, tmp);
            size_t offset = sizeof (F) - 1;
            for (int i = 0; i < sizeof (F); i++) {
                this->bytes[offset - i] = tmp >> (i << 3);
            }
            return value;
        }

        friend EFloat_Impl<F, I, true>;
    };

    template<typename F, typename I>
    class EFloat_Impl <F, I, true> : private TypeArray<F> {
        static_assert(sizeof (I) == sizeof (F), "Size of I is not equal size of F");
    public:

        inline EFloat_Impl() {
            operator=(0);
        }

        inline EFloat_Impl(F value) {
            operator=(value);
        }

        inline EFloat_Impl(const EFloat_Impl<F, I, false> &other) {
            size_t offset = sizeof (F) - 1;
            for (unsigned i = 0; i < sizeof (F); i++) {
                this->bytes[i] = other.bytes[offset - i];
            }
        }

        inline operator const F() const {
            I tmp = 0;
            F out;
            for (int i = 0; i < sizeof (F); i++) {
                tmp |= F(this->bytes[i]) << (i << 3);
            }
            _cast<sizeof (F)>(tmp, out);
            return out;
        }

        inline const F operator=(const F value) {
            I tmp = 0;
            _cast<sizeof (F)>(value, tmp);
            for (int i = 0; i < sizeof (F); i++) {
                this->bytes[i] = tmp >> (i << 3);
            }
            return value;
        }

        friend EFloat_Impl<F, I, false>;
    };

    template<typename T, typename S>
    class EContainer : public S {
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

        inline S operator++(int) {
            S tmp(*this);
            operator++();
            return tmp;
        }

        inline S& operator++() {
            T value = *this +1;
            *this = value;
            return *this;
        }

        inline S operator--(int) {
            S tmp(*this);
            operator--();
            return tmp;
        }

        inline S& operator--() {
            T value = *this -1;
            *this = value;
            return *this;
        }

        using S::operator=;
        using S::S;
    };

    template<typename I, bool le>
    using EInteger = EContainer<I, EInteger_Impl<I, le>>;

    template<typename F, typename I, bool le>
    using EFloat = EContainer<F, EFloat_Impl<F, I, le>>;
}

#endif /* ENDIAN_HPP */
