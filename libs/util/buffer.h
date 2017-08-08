#ifndef INCLUDED_BUFFER_H
#define INCLUDED_BUFFER_H

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace u {

    using byte = char;

    using cstring = const char *;

    namespace details {
        struct Reference {
        private:
            int m_size;
            byte *m_data;
        public:
            Reference(int m_size, byte *m_data)
                    : m_size(m_size), m_data(m_data) {}

            int size() const {
                return m_size;
            }

            const byte *data() const {
                return m_data;
            }

            byte *data() {
                return m_data;
            }
        };

        template<int sz>
        struct Value {
            byte m_data[sz];

            int size() const {
                return sz;
            }

            const byte *data() const {
                return m_data;
            }

            byte *data() {
                return m_data;
            }
        };
    }

    template<class Self>
    class IBuffer : Self {
        using Self::Self;
    public:
        operator const IBuffer<details::Reference>() const {
            return IBuffer<details::Reference>{this->size(), const_cast<byte *>(this->data())};
        }

        cstring c_str() const {
            return this->data();
        }

        operator cstring() const {
            return c_str();
        }

        std::size_t strlen() const {
            return ::strlen(c_str());
        }

        byte *mut() {
            return this->data();
        }

//        operator byte *() {
//            return mut();
//        }

        void terminate(long offset = -1) {
            if (offset < 0) {
                offset += this->size();
            }
            mut()[offset] = '\0';
        }

        void copy(cstring str, unsigned int offset = 0, unsigned int limit = 0) {
            if (!limit) {
                limit = this->size() - offset;
            }
            strncpy(mut() + offset, str, limit);
        }

        __attribute__((format(printf, 2, 3)))
        void sprintf(const char *format, ...) {
            // todo: check for overflow
            va_list argptr;
            va_start(argptr, format);
            vsprintf(this->data(), format, argptr);
            va_end(argptr);
        }
    };

    template<int sz>
    using BufferVal = IBuffer<details::Value<sz>>;

    using Buffer = IBuffer<details::Reference>;

    template<int sz>
    BufferVal<sz> buffer() {
        return BufferVal<sz>();
    }

}

#endif
