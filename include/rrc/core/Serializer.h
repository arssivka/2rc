/**
 *  @autor arssivka
 *  @date 12/19/16
 */

#pragma once


#include "rrc/meta.h"
#include <cstddef>
#include <type_traits>
#include <vector>
#include <list>
#include <cstdint>
#include <numeric>
#include <array>
#include <forward_list>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>

namespace rrc {
    struct SizeTag {};

    struct FixedSizeTag
            : public SizeTag {};

    struct DynamicSizeTag
            : public SizeTag {};

    using CharPointer = char*;
    using ConstCharPointer = char const*;
    using SizePointer = size_t*;
    using ConstSizePointer = size_t const*;
    
    template <class T>
    class Serializer {
    public:
        using SizeCategory = FixedSizeTag;
        using ValueTypes = MetaList<T>;
        
        using Type = std::remove_cv<T>;
        static_assert(!std::is_fundamental<Type>::value, "Can't deduce fundamental type");

        static void serialize(const Type& data, CharPointer& buffer) {
            // TODO: Check endianness
            *(T*) buffer = data;
            (T*) buffer += 1;
        };

        static bool deserialize(ConstCharPointer& buffer, ConstSizePointer& size, Type& data) {
            // TODO: Check endianness
            data = *(T*) buffer;
            size += 1;
            (T const*) buffer += 1;
            return true;
        }

        static size_t size(const Type& data) noexcept {
            return 1;
        }
    };


    template <class T>
    struct ContainerSerializer {
        using ValueTypes = MetaList<T>;
        using SizeCategory = DynamicSizeTag;

        static void serialize(const T& data, CharPointer& buffer) {
            for (auto&& item : data) {
                Serializer<decltype(item)>::serialize(item, buffer);
            }
        };

        size_t size(const T& data) noexcept {
            return data.size();
        }
    };

    template <class T>
    struct SequenceDeserializer {
        static bool deserialize(ConstSizePointer& buffer, ConstSizePointer& size, T& data) {
            data.resize(size[0]);
            size += 1;
            for (auto&& item : data) {
                if (!Serializer<decltype(item)>::deserialize(buffer, size, item)) return false;
            }
            return true;
        }
    };

    template <class T>
    struct AssociativeDeserializer {
        static bool deserialize(ConstCharPointer& buffer, ConstSizePointer& size, T& data) {
            size_t count = size[0];
            size += 1;
            for (size_t i = 0; i < count; ++i) {
                typename T::value_type value;
                if (!Serializer<T>::deserialize(buffer, size, value)) return false;
                data.insert(std::move(value));
            }
            return true;
        }
    };

    template<class T, size_t N>
    struct Serializer<T[N]>
            : public ContainerSerializer<T[N]> {
        static bool deserialize(ConstCharPointer& buffer, ConstSizePointer& size, T data[N]) {
            size_t total = size[0];
            size_t count = std::min(size[0], N);
            if (count > total) return false;
            size += 1;
            for (size_t i = 0; i < count; ++i) {
                if (!Serializer<T>::deserialize(buffer, size, data[i])) return false;
            }
            return true;
        }
    };

    template<class T, size_t N>
    struct Serializer<std::array<T, N>>
            : public ContainerSerializer<std::array<T, N>> {
        static bool deserialize(ConstCharPointer& buffer, ConstSizePointer& size, std::array<T, N>& data) {
            size_t total = size[0];
            size_t count = std::min(size[0], N);
            if (count > total) return false;
            size += 1;
            for (size_t i = 0; i < count; ++i) {
                if (!Serializer<T>::deserialize(buffer, size, data[i])) return false;
            }
            return true;
        }
    };

    template<class T, class Alloc>
    struct Serializer<std::vector<T, Alloc>>
            : public ContainerSerializer<std::vector<T, Alloc>>,
              public SequenceDeserializer<std::vector<T, Alloc>> {};

    template<class T, class Traits, class Alloc>
    struct Serializer<std::basic_string<T, Traits, Alloc>>
            : public ContainerSerializer<std::basic_string<T, Traits, Alloc>>,
              public SequenceDeserializer<std::basic_string<T, Traits, Alloc>> {};

    template<class T, class Alloc>
    struct Serializer<std::deque<T, Alloc>>
            : public ContainerSerializer<std::deque<T, Alloc>>,
              public SequenceDeserializer<std::deque<T, Alloc>> {};

    template<class T, class Alloc>
    struct Serializer<std::list<T, Alloc>>
            : public ContainerSerializer<std::list<T, Alloc>>,
              public SequenceDeserializer<std::list<T, Alloc>>  {};

    template<class T, class Alloc>
    struct Serializer<std::forward_list<T, Alloc>>
            : public ContainerSerializer<std::forward_list<T, Alloc>>,
              public SequenceDeserializer<std::forward_list<T, Alloc>>  {};

    template<class T, class Compare, class Alloc>
    struct Serializer<std::set<T, Compare, Alloc>>
            : public ContainerSerializer<std::set<T, Compare, Alloc>>,
              public AssociativeDeserializer<std::set<T, Compare, Alloc>> {};

    template<class T, class Compare, class Alloc>
    struct Serializer<std::multiset<T, Compare, Alloc>>
            : public ContainerSerializer<std::multiset<T, Compare, Alloc>>,
              public AssociativeDeserializer<std::multiset<T, Compare, Alloc>> {};

    template<class Key, class T, class Compare, class Alloc>
    struct Serializer<std::map<Key, T, Compare, Alloc>>
            : public ContainerSerializer<std::map<Key, T, Compare, Alloc>>,
              public AssociativeDeserializer<std::map<Key, T, Compare, Alloc>> {};

    template<class Key, class T, class Compare, class Alloc>
    struct Serializer<std::multimap<Key, T, Compare, Alloc>>
            : public ContainerSerializer<std::multimap<Key, T, Compare, Alloc>>,
              public AssociativeDeserializer<std::multimap<Key, T, Compare, Alloc>> {};

    template<class T, class Hash, class Pred, class Alloc>
    struct Serializer<std::unordered_set<T, Hash, Pred, Alloc>>
            : public ContainerSerializer<std::unordered_set<T, Hash, Pred, Alloc>>,
              public AssociativeDeserializer<std::unordered_set<T, Hash, Pred, Alloc>> {};

    template<class T, class Hash, class Pred, class Alloc>
    struct Serializer<std::unordered_multiset<T, Hash, Pred, Alloc>>
            : public ContainerSerializer<std::unordered_multiset<T, Hash, Pred, Alloc>>,
              public AssociativeDeserializer<std::unordered_multiset<T, Hash, Pred, Alloc>> {};

    template<class Key, class T, class Hash, class Pred, class Alloc>
    struct Serializer<std::unordered_map<Key, T, Hash, Pred, Alloc>>
            : public ContainerSerializer<std::unordered_map<Key, T, Hash, Pred, Alloc>>,
              public AssociativeDeserializer<std::unordered_map<Key, T, Hash, Pred, Alloc>> {};

    template<class Key, class T, class Hash, class Pred, class Alloc>
    struct Serializer<std::unordered_multimap<Key, T, Hash, Pred, Alloc>>
            : public ContainerSerializer<std::unordered_multimap<Key, T, Hash, Pred, Alloc>>,
              public AssociativeDeserializer<std::unordered_multimap<Key, T, Hash, Pred, Alloc>> {};

    template<class T, class K>
    struct Serializer<std::pair<T, K>> {
        using SizeCategory = DynamicSizeTag;
        using ValueTypes = MetaList<typename Serializer<T>::ValueTypes, typename Serializer<K>::ValueTypes>;

        static void serialize(const std::pair<T, K>& data, CharPointer& buffer) {
            Serializer<T>::serialize(data.first, buffer);
            Serializer<T>::serialize(data.second, buffer);
        };

        static bool deserialize(ConstCharPointer& buffer, ConstSizePointer& size, std::pair<T, K>& data) {
            return    Serializer<T>::deserialize(buffer, size, data.first)
                   && Serializer<T>::deserialize(buffer, size, data.second);
        }

        size_t size(const std::pair<T, K>& data) noexcept {
            return 1;
        }
    };

    template<class... Ts>
    struct Serializer<std::tuple<Ts...>> {
        using SizeCategory = FixedSizeTag;
        using ValueTypes = MetaList<typename Serializer<Ts>::ValueTypes...>;
        //TODO: This is not working. Guess what you need to do.
        
//        static void serialize(const std::tuple<Ts...>& data, CharPointer& buffer) {
//            for (size_t i = 0; i < sizeof...(Ts); ++i) {
//                using Arg = typename std::tuple_element<i, std::tuple<Ts...>>::type;
//                Serializer<Arg>::serialize(std::get<i>(data), buffer);
//            }
//        };
//
//        static bool deserialize(ConstCharPointer& buffer, ConstSizePointer& size, std::tuple<Ts...>& data) {
//            if (size[0] != Serializer<std::tuple<Ts...>>::minSize()) return false;
//            for (size_t i = 0; i < sizeof...(Ts); ++i) {
//                using Arg = typename std::tuple_element<i, std::tuple<Ts...>>::type;
//                if(!Serializer<Arg>::deserialize(buffer, size, std::get<i>(data))) return false;
//            }
//            return true;
//        }

        size_t size(const std::tuple<Ts...>& data) noexcept {
            return 1;
        }
    };

}
