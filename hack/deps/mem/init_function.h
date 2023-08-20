/*
    Copyright 2018 Brick

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software
    and associated documentation files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge, publish, distribute,
    sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or
    substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
    BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MEM_INIT_FUNCTION_BRICK_H
#define MEM_INIT_FUNCTION_BRICK_H

#include "defines.h"

namespace mem
{
    class init_function
    {
    public:
        using callback_t = void (*)();

        constexpr init_function() noexcept = default;
        init_function(callback_t callback) noexcept;
        init_function(init_function*& parent, callback_t callback) noexcept;

        init_function(const init_function&) = delete;
        init_function(init_function&&) = delete;

        static std::size_t init(init_function*& root = ROOT(), bool clear = true);

        static init_function*& ROOT() noexcept;

    private:
        callback_t callback_ {nullptr};
        init_function* next_ {nullptr};
    };

    MEM_STRONG_INLINE init_function::init_function(callback_t callback) noexcept
        : init_function(ROOT(), callback)
    {}

    MEM_STRONG_INLINE init_function::init_function(init_function*& parent, callback_t callback) noexcept
        : callback_(callback)
        , next_(parent)
    {
        parent = this;
    }

    MEM_STRONG_INLINE init_function*& init_function::ROOT() noexcept
    {
        static init_function* root {nullptr};

        return root;
    }

    MEM_STRONG_INLINE std::size_t init_function::init(init_function*& root, bool clear)
    {
        std::size_t total = 0;

        init_function* i = root;

        if (clear)
            root = nullptr;

        while (i)
        {
            if (i->callback_)
            {
                i->callback_();

                if (clear)
                    i->callback_ = nullptr;

                ++total;
            }

            init_function* j = i->next_;

            if (clear)
                i->next_ = nullptr;

            i = j;
        }

        return total;
    }
} // namespace mem

#endif // MEM_INIT_FUNCTION_BRICK_H
