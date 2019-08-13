// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <ecst/config.hpp>
#include <ecst/mp/list.hpp>
#include <ecst/signature/component/valid.hpp>

namespace ecst::signature_list::component
{
    template <typename T>
    constexpr auto valid(T&& x) noexcept
    {
        return bh::all_of(
            bh::transform(x, mp::unwrapped), signature::component::valid);
    }
} // namespace ecst::signature_list::component
