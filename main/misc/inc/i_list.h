#ifndef ILIST_H
#define ILIST_H

#include <cstdint>
#include <functional>

template<typename T>
class IList {
public:
    virtual uint16_t size() const = 0;
    virtual bool isEmpty() const = 0;
    virtual T get(uint16_t index) const = 0;
    T get() const {
        return get(size() - 1);
    };
    T getFirst() const {
        return get(0);
    };
    virtual void set(uint16_t index, T value) = 0;
    void set(T value) {
        set(size() - 1, value);
    };
    void setFirst(T value) {
        set(0, value);
    };
    virtual void add(uint16_t index, T value) = 0;
    void add(T value) {
        add(size(), value);
    };
    void addFirst(T value) {
        add(0, value);
    };
    virtual void add(const T* const src, uint16_t start, uint16_t quantity, uint16_t index) {
        for(uint16_t i = 0; i < quantity; i++){
            add(index + i, src[i + start]);
        }
    };
    void add(const T* const src, uint16_t len, uint16_t index) {
        add(src, 0, len, index);
    };
    void add(const T* const src, uint16_t len) {
        add(src, 0, len, size());
    };
    virtual void remove(uint16_t index, uint16_t quantity) = 0;
    void remove(uint16_t index) {
        remove(index, 1);
    };
    void remove() {
        remove(size() - 1);
    };
    T grab(uint16_t index) {
        T result = get(index);
        remove(index);
        return result;
    };
    T grab() const {
        return grab(size() - 1);
    };
    T grabFirst() const {
        return grab(0);
    };
    void grab(T* dst, uint16_t start, uint16_t quantity) {
        copyTo(dst, start, quantity);
        remove(start, quantity);
    };
    void grab(T* dst, uint16_t len) {
        copyTo(dst, len);
        remove(0, len);
    };
    void grab(T* dst) const {
        copyTo(dst, size());
        clear();
    };
    virtual int32_t firstIndexOf(T value) const = 0;
    virtual int32_t lastIndexOf(T value) const = 0;
    virtual void copyTo(T* dst, uint16_t start, uint16_t quantity) const = 0;
    void copyTo(T* dst, uint16_t len) {
        copyTo(dst, 0, len);
    };
    void copyTo(T* dst) const {
        copyTo(dst, size());
    };
    virtual void clear() = 0;
    virtual void fill(T value) = 0;
    virtual void forEach(std::function<void(T)> lambda) const = 0;
    virtual void forEachModify(std::function<T(T)> lambda) = 0;

};

#endif //ILIST_H
