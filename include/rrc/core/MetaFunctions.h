/**
 *  @autor arssivka
 *  @date 11/15/16
 */

#pragma once


#include <functional>

namespace rrc {
    namespace meta {
        /**
         * @brief Gives all the information about specified function.
         * You can recieve type of any argument. Returning type. Quantity of arguments.
         * Example of using: FunctionTraits<std::function<void(float, int)>>::Arg<0>
         * FunctionTraits<std::function<void(float, int)>>::Arg<1>
         * FunctionTraits<std::function<void(float, int)>>::Result
         * FunctionTraits<std::function<void(float, int)>>::count
         * @tparam T Needed function to extract traits. Function pointer can be applied too.
         */
        template<typename T>
        struct FunctionTraits;

        template<class... Args>
        struct List;

        template <bool Condition, class TrueType, class FalseType>
        using If = typename std::conditional<Condition, TrueType, FalseType>::type;

        template <class T>
        using Decay = typename std::decay<T>::type;

        template<typename Res, typename ...Args>
        struct FunctionTraits<std::function<Res(Args...)>> {
            static const size_t count = sizeof...(Args);
            using Result = Res;
            template<size_t i> using Arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };

        template<typename Res, typename ...Args>
        struct FunctionTraits<Res(*)(Args...)> {
            static const size_t count = sizeof...(Args);
            using Result = Res;
            template<size_t i> using Arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };

        /**
         * @brief Gives all the information about specified method.
         * You can recieve type of any argument. Returning type. Quantity of arguments.
         * Example of using: MethodTraits<decltype(&SomeClass::SomeFunc>::Arg<0>
         * FunctionTraits<decltype(&SomeClass::SomeFunc>::Arg<1>
         * FunctionTraits<decltype(&SomeClass::SomeFunc>::Result
         * FunctionTraits<decltype(&SomeClass::SomeFunc>::count
         * @tparam T Needed method to extract traits.
         */
        template<typename T>
        struct MethodTraits;
        template<typename Res, typename Cls, typename ...Args>
        struct MethodTraits<Res(Cls::*)(Args...)> {
            static const size_t count = sizeof...(Args);
            using Result = Res;
            using Class = Cls;
            template<size_t i> using Arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };

        namespace detail {
            template<class A, template<class...> class B>
            struct RenameImplementation;
            template<template<class...> class A, class... T,
                    template<class...> class B>
            struct RenameImplementation<A<T...>, B> {
                using Type = B<T...>;
            };
        }
        /**
         * @brief Renames class A to class B.
         * It can rename any variadic class template into any other and also non-variadic.
         * Example of using: Rename<std::vector<int>, std::list>
         * @tparam A Source class to rename
         * @tparam B Destination class
         */
        template<class A, template<class...> class B> using Rename = typename detail::RenameImplementation<A, B>::Type;

        template<class... T> using Length = typename std::integral_constant<std::size_t, sizeof...(T)>;
        template<class L> using Size = Rename<L, Length>;

        namespace detail {
            template<class L, class T>
            struct PushFrontImplementation;
            template<template<class...> class L, class... U, class... T>
            struct PushFrontImplementation<L<U...>, T...> {
                using Type = L<T..., U...>;
            };
        }
        /**
         * @brief Adds an element to the front of the List.
         * Example of using: PushFront<List<float>, int>
         * @tparam L List for adding the element.
         * @tparam T Element to add.
         */
        template<class L, class T> using PushFront = typename detail::PushFrontImplementation<L, T>::Type;


        namespace detail {
            template<class L>
            struct FrontImplementation;
            template<template<class...> class L, class T, class... U>
            struct FrontImplementation<L<T, U...>> {
                using Type = T;
            };
        }
        /**
         * @brief Gets the front element from the list.
         * Example of using: Front<List<float, int>>
         * @tparam L List to get element from
         */
        template<class L> using Front = typename detail::FrontImplementation<L>::Type;

        namespace detail {
            template<class L>
            struct PopFrontImplementation;
            template<template<class...> class L, class... U, class T>
            struct PopFrontImplementation<L<T, U...>> {
                using Type = L<U...>;
            };
        }
        /**
         * @brief Pops the front element from the list.
         * Example of using: PopFront<List<float, int>>
         * @tparam L List to pop element from.
         */
        template<class L> using PopFront = typename detail::PopFrontImplementation<L>::Type;


        using True = std::integral_constant<bool, true>;
        using False = std::integral_constant<bool, false>;


        namespace detail {
            template<class L>
            struct ClearImplementation;
            template<template<class...> class L, class... U>
            struct ClearImplementation<L<U...>> {
                using Type = L<>;
            };
        }
        /**
         * @brief Delets all the elements from list.
         * Example of using: Clear<List<int,int,int,int>>
         * @tparam L List to clear.
         */
        template<class L> using Clear = typename detail::ClearImplementation<L>::Type;

        namespace detail {
            template<class L>
            struct EmptyImplementation;
            template<template<class...> class L, class... U>
            struct EmptyImplementation<L<U...>> {
                constexpr static bool value = sizeof...(U) == 0;
            };
        }
        /**
         * @brief Checks if the specified list is empty or not
         * Example of using: Empty<List<int, int, int, int>>::value
         * @tparam L List to check.
         */
        template<class L> using Empty = detail::EmptyImplementation<L>;


        namespace detail {
            template<template<class...> class F, class L>
            struct TransformImplementation;
            template<template<class...> class F,
                    template<class...> class L, class... T>
            struct TransformImplementation<F, L<T...>> {
                using Type = L<F<T>...>;
            };
        }
        /**
         * @brief
         */
        template<template<class...> class F, class L> using Transform = typename detail::TransformImplementation<F, L>::Type;


        namespace detail {
            template<class... L>
            struct AppendImplementation;
            template<>
            struct AppendImplementation<> {
                using Type = List<>;
            };
            template<template<class...> class L, class... T>
            struct AppendImplementation<L<T...>> {
                using Type = L<T...>;
            };
            template<template<class...> class L1, class... T1,
                    template<class...> class L2, class... T2, class... Lr>
            struct AppendImplementation<L1<T1...>, L2<T2...>, Lr...> {
                using Type = typename AppendImplementation<L1<T1..., T2...>, Lr...>::Type;
            };
        }
        template<class... L> using Append = typename detail::AppendImplementation<L...>::Type;


        template<class T, T... Vals>
        struct IntegralSequence {};


        template<class T>
        struct BackSequenceElement { };
        template<class T, template<class, T...> class S, T First, T... Ss>
        struct BackSequenceElement<S<T, First, Ss...>> : public rrc::meta::If<sizeof...(Ss) == 0,
                std::integral_constant<T, First>,
                BackSequenceElement<S<T, Ss...>>> { };

        namespace detail {
            template<class T>
            struct BackImplementation {
            };
            template<template<class...> class S, class... Ss>
            struct BackImplementation<S<Ss...>> {
                using Type = typename std::tuple_element<sizeof...(Ss) - 1, std::tuple<Ss...>>::type;
            };
        }
        template<class T> using Back = typename detail::BackImplementation<T>::Type;



//        namespace detail {
//            template<class S>
//            struct GetSequenceImplementation;
//            template<template<class T, T... I> class S, class U, U... J>
//            struct GetSequenceImplementation<S<U, J...>> {
//                using Type = List<typename std::integral_constant<U, J>::value_type...>;
//            };
//        }
//        template<class S> using GetSequence = typename detail::GetSequenceImplementation<S>::Type;

        namespace detail {
            template<class A, class B>
            struct RenameSequenceImplementation;
            template<class T, template<class, T...> class A,
                    template<class, T...> class B, T... V>
            struct RenameSequenceImplementation<A<T, V...>, B<T>> {
                using Type = B<T, V...>;
            };
        }
        template<class A, class B> using RenameSequence = typename detail::RenameSequenceImplementation<A, B>::Type;


        namespace detail {
            template<class... S>
            struct AppendSequenceImplementation;
            template<class T, template<class, T...> class S, T... V>
            struct AppendSequenceImplementation<S<T, V...>> {
                using Type = S<T, V...>;
            };
            template<class T, template<class, T...> class S1, T... V1,
                    template<class, T...> class S2, T... V2, class... Lr>
            struct AppendSequenceImplementation<S1<T, V1...>, S2<T, V2...>, Lr...> {
                using Type = typename AppendSequenceImplementation<S1<T, V1..., V2...>, Lr...>::Type;
            };
        }
        template<class... S> using AppendSequence = typename detail::AppendSequenceImplementation<S...>::Type;


        template <class T, T First, T Second>
        struct Pack : std::integral_constant<T, (First << sizeof(T) * 4) | (Second & ((T)-1 >> sizeof(T) * 4))> { };


        namespace detail {
            template<bool I, class... Ss>
            struct PackerImplementation;
            template<class T, template<class, T...> class R,
                    template<class, T...> class V, T... Result, T First, T Second, T... Values, class... Ss>
            struct PackerImplementation<false, R<T, Result...>, V<T, First, Second, Values...>, Ss...> {
                using Type = typename PackerImplementation<false, R<T, Result...,
                        Pack<T, First, Second>::value>, V<T, Values...>, Ss...>::Type;
            };
            template<class T, template<class, T...> class R,
                    template<class, T...> class V, T... Result, T Last, class... Ss>
            struct PackerImplementation<false, R<T, Result...>, V<T, Last>, Ss...> {
                using Type = R<T, Result..., Pack<T, 0, Last>::value>;
            };
            template<class T, template<class, T...> class R,
                    template<class, T...> class V, T... Result, class... Ss>
            struct PackerImplementation<false, R<T, Result...>, V<T>, Ss...> {
                using Type = meta::If<meta::Length<Ss...>::value == 0, R<T, Result...>,
                        typename PackerImplementation<false, R<T, Result...>, Ss...>::Type>;
            };
            template<class T, template<class, T...> class R, T... Rs>
            struct PackerImplementation<false, R<T, Rs...>> {
                using Type = R<T, Rs...>;
            };
            template<class T, template<class, T...> class R, T... Ts, class... Os>
            struct PackerImplementation<true, R<T, Ts...>, Os...> {
                using Type = typename PackerImplementation<false, R<T>, R<T, Ts...>, Os...>::Type;
            };
        }
        template <class... Values>
        using Packer = typename detail::PackerImplementation<true, Values...>::Type;


        namespace detail {
            template<bool I, class... Ts>
            struct SequenceConcatenatorImplementation {
            };
            template<class T, template<class, T...> class R, template<class, T...> class S, T... Rs, T First, T... Ss, class... Os>
            struct SequenceConcatenatorImplementation<false, R<T, Rs...>, S<T, First, Ss...>, Os...> {
                using Type = typename rrc::meta::If<First == rrc::meta::BackSequenceElement<R<T, Rs...>>::value,
                        typename SequenceConcatenatorImplementation<false, R<T, Rs...>, S<T, Ss...>, Os...>::Type,
                        typename SequenceConcatenatorImplementation<false, R<T, Rs..., First>, S<T, Ss...>, Os...>::Type>;
            };
            template<class T, template<class, T...> class R, T... Ss,
                    template<class, T...> class S, T First, class... Os>
            struct SequenceConcatenatorImplementation<false, R<T>, S<T, First, Ss...>, Os...> {
                using Type = typename SequenceConcatenatorImplementation<false, R<T, First>, S<T, Ss...>, Os...>::Type;

            };
            template<class T, template<class, T...> class R, template<class, T...> class S, T... Rs, class... Os>
            struct SequenceConcatenatorImplementation<false, R<T, Rs...>, S<T>, Os...> {
                using Type = typename rrc::meta::If<Length<Os...>::value == 0,
                        R<T, Rs...>,
                        typename SequenceConcatenatorImplementation<false, R<T, Rs...>, Os...>::Type>;
            };
            template<class T, template<class, T...> class R, T... Rs>
            struct SequenceConcatenatorImplementation<false, R<T, Rs...>> {
                using Type = R<T, Rs...>;
            };
            template<class T, template<class, T...> class R, T... Ts, class... Os>
            struct SequenceConcatenatorImplementation<true, R<T, Ts...>, Os...> {
                using Type = typename SequenceConcatenatorImplementation<false, R<T>, R<T, Ts...>, Os...>::Type;
            };
        }
        template <class... Ts>
        using SequenceConcatenator = typename detail::SequenceConcatenatorImplementation<true, Ts...>::Type;


        namespace detail {
            template<bool I, class... Ts>
            struct ConcatenatorImplementation {
            };
            template<template<class...> class R, template<class...> class S, class... Rs, class First, class... Ss, class... Os>
            struct ConcatenatorImplementation<false, R<Rs...>, S<First, Ss...>, Os...> {
                using Type = typename rrc::meta::If<std::is_same<First, rrc::meta::Back<R<Rs...>>>::value,
                        typename ConcatenatorImplementation<false, R<Rs...>, S<Ss...>, Os...>::Type,
                        typename ConcatenatorImplementation<false, R<Rs..., First>, S<Ss...>, Os...>::Type>;
            };
            template<template<class...> class R, class... Ss,
                    template<class...> class S, class First, class... Os>
            struct ConcatenatorImplementation<false, R<>, S<First, Ss...>, Os...> {
                using Type = typename ConcatenatorImplementation<false, R<First>, S<Ss...>, Os...>::Type;

            };
            template<template<class...> class R, template<class...> class S, class... Rs, class... Os>
            struct ConcatenatorImplementation<false, R<Rs...>, S<>, Os...> {
                using Type = typename rrc::meta::If<Length<Os...>::value == 0,
                        R<Rs...>,
                        typename ConcatenatorImplementation<false, R<Rs...>, Os...>::Type>;
            };
            template<template<class...> class R, class... Rs>
            struct ConcatenatorImplementation<false, R<Rs...>> {
                using Type = R<Rs...>;
            };
            template<template<class...> class R, class... Ts, class... Os>
            struct ConcatenatorImplementation<true, R<Ts...>, Os...> {
                using Type = typename ConcatenatorImplementation<false, R<>, R<Ts...>, Os...>::Type;
            };
        }
        template <class... Ts>
        using Concatenator = typename detail::ConcatenatorImplementation<true, Ts...>::Type;


        namespace detail {
            template <class S>
            struct ExtractTypeImplementation;
            template <template <class> class S, class T>
            struct ExtractTypeImplementation<S<T>> {
                using Type = T;
            };
        }
        template <class S>
        using ExtractType = typename detail::ExtractTypeImplementation<S>::Type;

        template<std::size_t I = 0, typename FuncT, typename... Tp>
        inline typename std::enable_if<I == sizeof...(Tp), void>::type
        forEach(std::tuple<Tp...> &, FuncT) { }

        template<std::size_t I = 0, typename FuncT, typename... Tp>
        inline typename std::enable_if<I < sizeof...(Tp), void>::type
        forEach(std::tuple<Tp...>& t, FuncT f) {
            f(std::get<I>(t));
            forEach<I + 1, FuncT, Tp...>(t, f);
        }
    }
}

