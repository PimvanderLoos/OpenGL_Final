#ifndef GAME_MYSTORAGE_H
#define GAME_MYSTORAGE_H

#include <iosfwd>
#include <mutex>
#include <shared_mutex>

template<typename Data>
class MyStorage
{
    private:
        Data **d_data;
        size_t d_objectCount;
        size_t d_objectCapacity;
//        mutable std::shared_mutex d_mutex;

        // Backend reserve (without mutex locking)
        void privateReserve(size_t size);

    public:
        MyStorage();
        ~MyStorage();
        void append(Data *obj);
        void append(Data &obj);
        void addObject(Data *obj);
        void removeAt(size_t pos);
        void reserve(size_t size);

        size_t size() const;
        size_t capacity() const;
        Data *createNewObject();
        Data *at(size_t idx);
        Data const *at(size_t idx) const;

        Data *last();
        Data const *last() const;

        Data *operator[](size_t idx);
        Data const *operator[](size_t idx) const;

        Data *begin();
        Data *end();
};

template<typename Data>
inline Data *MyStorage<Data>::begin()
{
    return d_data[0];
}

template<typename Data>
inline Data *MyStorage<Data>::end()
{
    return d_data[0];
}

template<typename Data>
inline Data *MyStorage<Data>::operator[](size_t idx)
{   Q_ASSERT_X(idx < d_objectCount, "MyStorage<T>::operator[]", "index out of range");
//    std::shared_lock lock(d_mutex);
    return d_data[idx];
}

template<typename Data>
inline Data const *MyStorage<Data>::operator[](size_t idx) const
{   Q_ASSERT_X(idx < d_objectCount, "MyStorage<T>::operator[]", "index out of range");
//    std::shared_lock lock(d_mutex);
    return d_data[idx];
}

template<typename Data>
inline MyStorage<Data>::MyStorage()
:
    d_data(new Data*[1]),
    d_objectCount(0),
    d_objectCapacity(1)
{
    qDebug() << "MyStorage constructor!";
}

template<typename Data>
inline MyStorage<Data>::~MyStorage()
{
    for (size_t idx = 0; idx != d_objectCount; ++idx)
        delete d_data[idx];
    delete[] d_data;
}

template<typename Data>
inline void MyStorage<Data>::addObject(Data *obj)
{
    append(obj);
}

template<typename Data>
inline void MyStorage<Data>::append(Data *obj)
{
    // Make more space if needed.
    if (d_objectCount == d_objectCapacity)
        privateReserve(d_objectCapacity);

    d_data[d_objectCount] = obj;
    ++d_objectCount;
}

template<typename Data>
inline void MyStorage<Data>::append(Data &obj)
{
    append(&obj);
}

// Delete the object at the given position and move the last object to the current position.
template<typename Data>
inline void MyStorage<Data>::removeAt(size_t pos)
{
    if (pos >= d_objectCount)
        return;
//    std::unique_lock lock(d_mutex);
    delete d_data[pos];
    d_data[pos] = d_data[--d_objectCount];
}

// Add 'size' more data storage.
template<typename Data>
inline void MyStorage<Data>::privateReserve(size_t size)
{
    if (size == 0)
        return;

//    std::unique_lock lock(d_mutex);
    auto *tmp = new Data*[d_objectCapacity + size];

    for (size_t idx = 0; idx != d_objectCount; ++idx)
        tmp[idx] = d_data[idx];
    delete[] d_data;

    d_data = tmp;
    d_objectCapacity += size;
}

// Add 'size' more data storage.
template<typename Data>
inline void MyStorage<Data>::reserve(size_t size)
{
    if (size == 0)
        return;

//    std::unique_lock lock(d_mutex);
    privateReserve(size);
}

template<typename Data>
inline size_t MyStorage<Data>::size() const
{
    return d_objectCount;
}

template<typename Data>
inline size_t MyStorage<Data>::capacity() const
{
    return d_objectCapacity;
}

template<typename Data>
inline Data *MyStorage<Data>::createNewObject()
{
//    std::unique_lock lock(d_mutex);
    auto *obj = new Data();
    append(obj);
    return obj;
}

template<typename Data>
inline Data *MyStorage<Data>::at(size_t idx)
{   Q_ASSERT_X(idx < d_objectCount, "MyStorage<T>::at", "index out of range");
//    std::shared_lock lock(d_mutex);
    return d_data[idx];
}

template<typename Data>
inline Data const *MyStorage<Data>::at(size_t idx) const
{   Q_ASSERT_X(idx < d_objectCount, "MyStorage<T>::at", "index out of range");
    return at(idx);
}

template<typename Data>
inline Data *MyStorage<Data>::last()
{   Q_ASSERT_X(d_objectCount > 0, "MyStorage<T>::last", "No objects in storage");
    return at(d_objectCount - 1);
}

template<typename Data>
inline Data const *MyStorage<Data>::last() const
{   Q_ASSERT_X(d_objectCount > 0, "MyStorage<T>::last", "No objects in storage");
    return at(d_objectCount - 1);
}

#endif
