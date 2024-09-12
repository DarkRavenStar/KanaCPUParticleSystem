#pragma once

#include <algorithm>
#include <tuple>
#include "SpanHelper.h"

// Helper function to pack containers into spans into tuples
// Using std::tuple for now is fine, but using
// it on larger might cause compilation and
// performance impact revisit this if the
// project expands in scope
namespace MultiSpanHelper
{
    //Need to be used with containers of same size or it will use the smallest element count
    template <typename... Containers>
    auto GetZipped(const Containers&... containers)
    {
        //using TupleType = std::tuple<decltype(*std::begin(containers))...>;
		//using ReturnType = std::vector<TupleType>;
        
        std::vector<std::tuple<decltype(*std::begin(containers))...>> zipped;

        auto spanArrayTuple = std::make_tuple(SpanHelper::MakeSpan(containers)...);
        
        std::size_t minSize = std::min<std::size_t>({ std::size(containers)... });
        
        for (std::size_t i = 0; i < minSize; i++)
        {
            zipped.emplace_back
            (
                std::apply([i](auto&... spans) { return std::tuple<decltype(*std::begin(containers))...>{ spans[i]... }; }, spanArrayTuple)
            );
        }
        
        return zipped;
    }
}