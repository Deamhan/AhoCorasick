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

    template <class StringType, PerformanceStrategy strategy = PerformanceStrategy::Balanced>
    class Scanner
    {
    public:
        typedef typename StringType::value_type ValueType;

        template <class MatchCallback, class InputIt>
        void Scan(const MatchCallback& callback, InputIt begin, InputIt end)
        {
            mImpl->Scan(callback, begin, end);
        }

        static const PerformanceStrategy appliedStrategy = GetPerformanceStrategy<ValueType>(strategy);

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
