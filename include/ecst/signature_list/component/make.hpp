// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <ecst/config.hpp>
#include <ecst/mp/list.hpp>
#include <ecst/signature/component.hpp>

namespace ecst::signature_list::component
{
    template <typename... TComponentSignatures>
    constexpr auto make(TComponentSignatures... css)
    {
        ECST_S_ASSERT_DT(signature::component::valid(css...));
        return mp::list::make(mp::wrap(css)...);
    }
} // namespace ecst::signature_list::component
