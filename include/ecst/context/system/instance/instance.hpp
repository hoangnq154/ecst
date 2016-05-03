// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <vector>
#include <ecst/config.hpp>
#include <ecst/aliases.hpp>
#include <ecst/utils/sparse_int_set.hpp>
#include <ecst/thread_pool.hpp>
#include <ecst/mp.hpp>
#include <ecst/signature.hpp>
#include <ecst/signature_list.hpp>
#include <ecst/inner_parallelism.hpp>
#include <ecst/settings.hpp>
#include <ecst/context/types.hpp>
#include <ecst/context/bitset.hpp>
#include "./../state.hpp"

ECST_CONTEXT_SYSTEM_NAMESPACE
{
    template <typename TSettings, typename TSystemSignature>
    class instance
    {
    private:
        using system_tag_type =
            signature::system::impl::signature_tag_type<TSystemSignature>;

        using system_type = signature::system::unwrap_tag<system_tag_type>;

        using bitset_type = bitset::dispatch<TSettings>;

        using state_manager_type = // .
            impl::state_manager::dispatch<TSettings, TSystemSignature>;

        using set_type = dispatch_set<TSettings>;

        using parallel_parameters_type = // .
            signature::system::parallelism_policy<TSystemSignature>;

        using parallel_executor_type = // .
            inner_parallelism::executor_type<parallel_parameters_type>;

        system_type _system;
        state_manager_type _sm;
        set_type _subscribed;
        bitset_type _bitset;
        parallel_executor_type _parallel_executor;

    public:
        ECST_ALWAYS_INLINE auto& ECST_PURE_FN subscribed() noexcept
        {
            return _subscribed;
        }

        ECST_ALWAYS_INLINE const auto& ECST_PURE_FN subscribed() const noexcept
        {
            return _subscribed;
        }

    public:
        static constexpr auto system_id() noexcept
        {
            return signature_list::system::id_by_type<system_type>(
                settings::system_signature_list(TSettings{}));
        }

        instance();

        instance(const instance&) = delete;
        instance& operator=(const instance&) = delete;

        instance(instance&&) = default;
        instance& operator=(instance&&) = default;

        /// @brief Executes `f` on all subtask states.
        template <typename TF>
        decltype(auto) for_states(TF&& f);

        /// @brief Executes all subtask states' deferred functions.
        template <typename TProxy>
        void execute_deferred_fns(TProxy& proxy);

        /// @brief Executes `f` on all subtask outputs.
        template <typename TF>
        decltype(auto) for_outputs(TF&& f);

        /// @brief Left-folds over `acc` with `f` on all subtask outputs.
        template <typename TAcc, typename TF>
        auto foldl_outputs(TAcc acc, TF&& f);

        /// @brief Returns `true` if `eid` is subscribed to the system.
        auto is_subscribed(entity_id eid) const noexcept;

        /// @brief Subscribes `eid` to the system.
        auto subscribe(entity_id eid);

        /// @brief Unsubscribes `eid` from the system.
        auto unsubscribe(entity_id eid);

        auto& system() noexcept
        {
            return _system;
        }

        const auto& system() const noexcept
        {
            return _system;
        }

        const auto& bitset() const noexcept
        {
            return _bitset;
        }

        template <typename TBitset>
        auto matches_bitset(const TBitset& b) const noexcept
        {
            return _bitset.contains(b);
        }

    public:
        template <                          // .
            typename TFEntityProvider,      // .
            typename TFAllEntityProvider,   // .
            typename TFOtherEntityProvider, // .
            typename TContext,              // .
            typename TFStateGetter          // .
            >
        auto make_data(                    // .
            TFEntityProvider&& f_ep,       // .
            TFAllEntityProvider&& f_aep,   // .
            TFOtherEntityProvider&& f_oep, // .
            TContext& ctx,                 // .
            TFStateGetter&& sg             // .
            );

        /// @brief Clears and allocates `n` subtask states.
        template <typename TF>
        void prepare_and_wait_n_subtasks(sz_t n, TF&& f);

        /// @brief Executes `f` on a subset of the subscribed entities and
        /// decrements the atomic remaining subtasks counter.
        template <typename TCounterBlocker, typename TData, typename TF>
        void execute_subtask_and_decrement_counter(
            TCounterBlocker& cb, TData& data, TF&& f);

        /// @brief Executes `f` on every subscribed entity, without spawning any
        /// additional task.
        template <typename TContext, typename TF>
        void execute_single(TContext& ctx, TF&& f);

        /// @brief Executes `f` using the strategy provided by
        /// `_parallel_executor`.
        template <typename TContext, typename TF>
        void execute_in_parallel(TContext& ctx, TF&& f);

    public:
        template <typename TContext, typename TF>
        void execute(TContext& ctx, TF&& f);

        auto subscribed_count() const noexcept
        {
            return this->subscribed().size();
        }

        auto nth_subscribed(sz_t n) noexcept;
        auto& nth_state(sz_t n) noexcept;

        auto make_all_entity_provider() noexcept;
        auto make_entity_range_provider(sz_t i_begin, sz_t i_end) noexcept;
        auto make_other_entity_range_provider(
            sz_t i_begin, sz_t i_end) noexcept;

        template <typename TContext>
        auto make_entity_range_data(
            TContext& ctx, sz_t state_idx, sz_t i_begin, sz_t i_end) noexcept;

        template <typename TCounterBlocker, typename TContext>
        auto make_slice_executor(TCounterBlocker& cb, TContext& ctx,
            sz_t state_idx, sz_t i_begin, sz_t i_end) noexcept;

        template <typename TCounterBlocker, typename TContext, typename TF>
        auto make_bound_slice_executor(TCounterBlocker& cb, TContext& ctx,
            sz_t state_idx, sz_t i_begin, sz_t i_end, TF&& f) noexcept;

        template <typename TContext, typename TF>
        auto run_subtask_in_thread_pool(TContext& ctx, TF&& f);
    };
}
ECST_CONTEXT_SYSTEM_NAMESPACE_END
