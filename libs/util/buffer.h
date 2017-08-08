#ifndef INCLUDED_BUFFER_H
#define INCLUDED_BUFFER_H

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

        operator byte *() {
            return this->data();
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
