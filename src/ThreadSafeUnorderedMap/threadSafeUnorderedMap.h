#ifndef GAME_THREADSAFEUNORDEREDMAP_H
#define GAME_THREADSAFEUNORDEREDMAP_H

#include <unordered_map>
#include <shared_mutex>

template<typename Key, typename Value,
        typename Hash = std::hash<Key>,
        typename Pred = std::equal_to<Key>,
        typename Allocator = std::allocator<std::pair<const Key, Value>>>
class ThreadSafeUnorderedMap : private std::unordered_map<Key, Value, Hash, Pred, Allocator>
{
    private:
        mutable std::shared_mutex d_mutex;

    public:
        void insert(std::pair<Key, Value> const &data);
        void insert(std::pair<Key, Value> &&data);
        void insert(Key const &key, Value const &value);
        void insert(Key &&key, Value &&value);

        auto insert_or_assign(std::pair<Key, Value> const &data);
        auto insert_or_assign(std::pair<Key, Value> &&data);
        auto insert_or_assign(Key const &key, Value const &value);
        auto insert_or_assign(Key &&key, Value &&value);

        auto find(Key const &key) const;
        auto find(Key const &key);

        typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::const_iterator end() const;
        typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::const_iterator begin() const;
        typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::iterator end();
        typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::iterator begin();

        size_t size() const;

        typedef typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::const_iterator const_iterator;
        typedef typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::iterator iterator;
};

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline void ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert(std::pair<Key, Value> const &data)
{
    std::unique_lock lock(d_mutex);
    std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert(data);
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline void ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert(std::pair<Key, Value> &&data)
{
    std::unique_lock lock(d_mutex);
    std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert(data);
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline void ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert(Key const &key, Value const &value)
{
    std::unique_lock lock(d_mutex);
    std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert({key, value});
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline void ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert(Key &&key, Value &&value)
{
    std::unique_lock lock(d_mutex);
    std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert({key, value});
}


template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline auto ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert_or_assign(std::pair<Key, Value> const &data)
{
    std::unique_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert_or_assign(data);
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline auto ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert_or_assign(std::pair<Key, Value> &&data)
{
    std::unique_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert_or_assign(data);
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline auto ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert_or_assign(Key const &key, Value const &value)
{
    std::unique_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert_or_assign({key, value});
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline auto ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::insert_or_assign(Key &&key, Value &&value)
{
    std::unique_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::insert_or_assign({key, value});
}


template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline auto ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::find(Key const &key) const
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::find(key);
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline auto ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::find(Key const &key)
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::find(key);
}


template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::const_iterator ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::begin() const
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::begin();
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::iterator ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::begin()
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::begin();
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::const_iterator ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::end() const
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::end();
}

template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline typename std::unordered_map<Key, Value, Hash, Pred, Allocator>::iterator ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::end()
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::end();
}


template<typename Key, typename Value, typename Hash, typename Pred, typename Allocator>
inline size_t ThreadSafeUnorderedMap<Key, Value, Hash, Pred, Allocator>::size() const
{
    std::shared_lock lock(d_mutex);
    return std::unordered_map<Key, Value, Hash, Pred, Allocator>::size();
}


#endif
