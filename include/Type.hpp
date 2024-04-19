/**
 * @file Type.hpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef TYPE_PACS
#define TYPE_PACS

// Concepts.
#include <concepts>

// Math.
#include <complex>
#include <cmath>

namespace pacs {

    namespace algebra {

        /**
         * @brief Addable types.
         * 
         * @tparam T 
         */
        template<typename T>
        concept Addable = requires(T first, T second) {
            {first += second} -> std::convertible_to<T>;
            {first -= second} -> std::convertible_to<T>;
        };

        /**
         * @brief Multipliable types.
         * 
         * @tparam T 
         */
        template<typename T>
        concept Multipliable = requires(T first, T second) {
            {first *= second} -> std::convertible_to<T>;
            {first /= second} -> std::convertible_to<T>;
        };

        /**
         * @brief Absolute value supported types.
         * 
         * @tparam T 
         */
        template<typename T>
        concept Absolute = requires(T value) {
            {std::abs(value)} -> std::convertible_to<double>;
        };

        /**
         * @brief Matrix' type.
         * 
         * @tparam T 
         */
        template<typename T>
        concept MatrixType = Addable<T> && Multipliable<T> && Absolute<T>;
    }

}

#endif