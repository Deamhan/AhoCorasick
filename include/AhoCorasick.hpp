/**
 * \file AhoCorasick.hpp
 *
 * \brief All-in-one Aho-Corasick pattern searching algorithm implementation. 
 *
 * \copyright Copyright (C) 2023 Pavel Kovalenko. All rights reserved.<br>
 * <br>
 * This Source Code Form is subject to the terms of the Mozilla<br>
 * Public License, v. 2.0. If a copy of the MPL was not distributed<br>
 * with this file, you can obtain one at http://mozilla.org/MPL/2.0/.
 */


#pragma once

#include <array>
#include <cstdint>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <queue>

namespace AhoCorasick
{
    /**
     * \brief Helper class used to report succesful match. 
     * 
     * \tparam StringType Pattern holding container class supporting iterators
     *
     */
    template <class StringType>
    struct Match
    {
        size_t offset;
        size_t index;
        const StringType* word;

        Match(size_t offsetArg, size_t indexArg, const StringType* wordArg) noexcept : 
            offset(offsetArg), index(indexArg), word(wordArg)
        {}
    };

    /**
     * \brief Allows to select max performance or balnce between perf and memory consumption 
     * 
     * Strategy defines how child nodes will be stored inside parent nodes. For <em>MaximumPerformance</em> children stored inside array 
     * with all possible 'character' codes inside making it memory consuming, but fast (access by index with <em>O(1)</em>). 
     * Otherwise <em>Balanced</em> uses <em>std::map</em> as children container. <em>MaximumPerformance</em> can be enabled only for 1 byte 
     * sized integer types (for others it will be changed to <em>Balanced</em>).
     *
     */
    enum class PerformanceStrategy
    {
        MaximumPerformance,
        Balanced
    };

    template <class StringType, PerformanceStrategy userStrategy = PerformanceStrategy::Balanced>
    class ScannerImpl;

    template <class ValueType>
    constexpr size_t CanUseMaximumPerformancePolicy() { return sizeof(ValueType) == 1; }

    template <class ValueType>
    constexpr std::enable_if_t<std::numeric_limits<ValueType>::is_integer, PerformanceStrategy>  GetPerformanceStrategy(PerformanceStrategy strategy)
    {
        return CanUseMaximumPerformancePolicy<ValueType>() ? strategy : PerformanceStrategy::Balanced;
    }

    template <class ValueType>
    constexpr PerformanceStrategy GetPerformanceStrategy(...) { return PerformanceStrategy::Balanced; }

    /**
     * \brief Main class allowing to scan sequence of 'characters' for patterns. Doesn't support empty sequences and duplications.
     * 
     * \tparam StringType Pattern holding container class supporting STL style iterators
     * \tparam strategy Strategy used for a new Scanner
     *
     * Actually the only class you need to use the implementation. If you have a small amount of char/byte patterns I recommend to use 
     * <em>MaximumPerformance</em> strategy.
     *
     */
    template <class StringType, PerformanceStrategy strategy = PerformanceStrategy::Balanced>
    class Scanner
    {
    public:
        typedef typename StringType::value_type ValueType;

        /**
         * \brief Scans for patterns using callback provided.
         *
         * \tparam MatchCallback Function-like callback of bool(::Match)
         * \tparam InputIt Iterator-like class holding 'character' to scan
         * 
         * \param callback Callback of type ::MatchCallback
         * \param begin First input sequence iterator
         * \param end Last input sequence iterator
         *
         * Return value of the callback defines if scanning should continue or not.
         *
         */
        template <class MatchCallback, class InputIt>
        void Scan(const MatchCallback& callback, InputIt begin, InputIt end)
        {
            mImpl->Scan(callback, begin, end);
        }

        static const PerformanceStrategy appliedStrategy = GetPerformanceStrategy<ValueType>(strategy);

        /**
         * \brief Scanner constructor consuming patterns to search.
         *
         * \tparam WordIt Pattern collection iterator type
         * 
         * \param begin First iterator
         * \param end Last iterator
         *
         * Patterns must be stored in collection supporting iteration (can be a simple array). 
         * Collection item type must be <em>StringType</em>.
         *
         */
        template <class WordIt>
        Scanner(WordIt begin, WordIt end) : 
            mImpl(std::make_unique<ScannerImpl<StringType, appliedStrategy>>(begin, end))
        {}

    private:
        std::unique_ptr<ScannerImpl<StringType, appliedStrategy>> mImpl;
    };

#pragma region Implementation

    template<class ValueType, class StringType, PerformanceStrategy strategy>
    struct TrieNode;

    enum class NodeSearchResult
    {
        Added,
        Found
    };

    template <class ValueType, class StringType, PerformanceStrategy strategy>
    struct NodeChildren
    {
        typedef TrieNode<ValueType, StringType, strategy> NodeType;
        typedef std::map<ValueType, NodeType> MapType;

        NodeSearchResult AddOrGet(const ValueType& value, NodeType*& result) noexcept
        {
            auto it = nodes.lower_bound(value);
            bool found = (it != nodes.end() && it->first == value);
            if (found)
            {
                result = &it->second;
                return NodeSearchResult::Found;
            }

            auto added = nodes.emplace_hint(it, value, NodeType{});
            result = &added->second;
            result->value = value;
            return NodeSearchResult::Added;
        }

        NodeType* TryGet(const ValueType& value) noexcept
        {
            auto it = nodes.find(value);
            return it != nodes.end() ? &it->second : nullptr;
        }

        MapType nodes;

        NodeType* GetFromStorageSlot(typename MapType::value_type& storageSlot) noexcept { return &storageSlot.second; }
    };

    template <class ValueType, class StringType>
    struct NodeChildren<ValueType, StringType, PerformanceStrategy::MaximumPerformance>
    {
        static const PerformanceStrategy strategy = PerformanceStrategy::MaximumPerformance;

        typedef TrieNode<ValueType, StringType, strategy> NodeType;
        typedef std::array<std::unique_ptr<NodeType>, std::numeric_limits<ValueType>::max()> ArrayType;
        ArrayType nodes;

        NodeType* TryGet(const ValueType& value)
        {
            return nodes[(size_t)value].get();
        }

        template <class PointerType>
        void Add(const ValueType& value, PointerType&& node, NodeType*)
        {
            auto& slot = nodes[(size_t)value];
            slot = std::forward<PointerType>(node);

        }

        NodeSearchResult AddOrGet(const ValueType& value, NodeType*& result) noexcept
        {
            auto item = TryGet(value);
            if (item != nullptr)
            {
                result = item;
                return NodeSearchResult::Found;
            }

            auto& slot = nodes[(size_t)value]; 
            slot = std::make_unique<NodeType>();
            result = slot.get();
            result->value = value;
            return NodeSearchResult::Added;
        }

        NodeType* GetFromStorageSlot(typename ArrayType::value_type& storageSlot) noexcept { return storageSlot.get(); }
    };

    template<class ValueType, class StringType, PerformanceStrategy strategy>
    struct TrieNode
    {
        size_t matchIndex;
        TrieNode* failureLink;
        TrieNode* nextMatchLink;
        TrieNode* parentLink;
        ValueType value;
        StringType word;

        static const size_t InvalidMatchIndex = std::numeric_limits<size_t>::max();

        NodeChildren<ValueType, StringType, strategy> children;

        TrieNode() noexcept : matchIndex(InvalidMatchIndex), failureLink(nullptr), nextMatchLink(nullptr),
            parentLink(nullptr), value(ValueType())
        {}
    };

    template <class StringType, PerformanceStrategy strategy>
    class ScannerImpl
    {
    public:
        typedef typename StringType::value_type ValueType;

        template <class MatchCallback, class InputIt>
        void Scan(const MatchCallback& callback, InputIt begin, InputIt end)
        {
            NodeType* current = &mRoot;
            size_t offset = 0;
            for (InputIt next = begin; next != end; ++next, ++offset)
            {
                current = FindNextCharNode(*next, current);
                if (current == nullptr)
                {
                    current = &mRoot;
                    continue;
                }

                auto matchNode = current;
                do
                {
                    if (!matchNode->word.empty())
                    {
                        Match<StringType> m { offset + 1 - matchNode->word.size(), matchNode->matchIndex, &matchNode->word };
                        if (!callback(m))
                            return;
                    }

                    matchNode = matchNode->nextMatchLink;
                } while (matchNode != nullptr);
            }
        }

        template <class WordIt>
        ScannerImpl(WordIt begin, WordIt end) : mCurrentIndex(0)
        {
            for (WordIt it = begin; it < end; ++it)
                AddWord(*it);

            BuildLinks();
        }

    private:
        typedef TrieNode<ValueType, StringType, strategy> NodeType;
        NodeType mRoot;
        size_t mCurrentIndex;

        NodeType* FindNextCharNode(const ValueType& chr, NodeType* parent)
        {
            NodeType* result = parent;
            while (result != nullptr)
            {
                NodeType* nextLink = result->children.TryGet(chr);
                if (nextLink != nullptr)
                {
                    result = nextLink;
                    break;
                }

                result = result->failureLink;
            }

            return result;
        }

        void BuildChildLink(NodeType* child)
        {
            auto chr = child->value;
            NodeType* failureLink = child->parentLink->failureLink;
            while (failureLink != nullptr)
            {
                NodeType* nextLink = failureLink->children.TryGet(chr);
                if (nextLink != nullptr)
                {
                    failureLink = nextLink;
                    break;
                }

                failureLink = failureLink->failureLink;
            }

            if (failureLink != nullptr)
            {
                child->failureLink = failureLink;
                child->nextMatchLink = failureLink->word.empty() ? failureLink->nextMatchLink : failureLink;
            }
            else
                child->failureLink = &mRoot;
        }

        void BuildLinks()
        {
            std::queue<NodeType*> queue;
            queue.push(&mRoot);
            do
            {
                auto node = queue.front();
                queue.pop();

                for (auto& slot : node->children.nodes)
                {
                    auto ptr = node->children.GetFromStorageSlot(slot);
                    if (ptr == nullptr)
                        continue;

                    BuildChildLink(ptr);
                    queue.push(ptr);
                }
            } while (!queue.empty());
        }

        bool AddWord(const StringType& word)
        {
            if (word.empty())
                return false;

            auto* currentNode = &mRoot;
            for (const auto& c : word)
            {
                NodeType* found = nullptr;
                if (currentNode->children.AddOrGet(c, found) == NodeSearchResult::Added)
                    found->parentLink = currentNode;

                currentNode = found;
            }

            if (!currentNode->word.empty())
                return false;

            currentNode->matchIndex = mCurrentIndex++;
            currentNode->word = word;

            return true;
        }
    };

#pragma endregion Implementation
}
