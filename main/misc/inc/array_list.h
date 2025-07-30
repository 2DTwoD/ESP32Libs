#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <cstdint>
#include "math_fun.h"
#include "array_fun.h"
#include "i_list.h"

template<typename T>
class ArrayList: public IList<T>{
private:
    uint8_t increase;
    uint16_t totalSize;
    uint16_t curIndex{0};
    T zeroValue{};
    T* array{nullptr};

    void extend(){
        totalSize += increase;
        T* newArray = new T[totalSize];
        copyArrays(array, newArray, size());
        delete[] array;
        array = newArray;
    }
protected:
    T carefulGet(uint16_t index){
        return array[index];
    }
public:
    using IList<T>::get;
    using IList<T>::set;
    using IList<T>::add;
    using IList<T>::remove;
    using IList<T>::grab;
    using IList<T>::copyTo;

    ArrayList(uint8_t increaseStep): IList<T>() {
        increase = max((uint8_t )1, increaseStep);
        totalSize = increase;
        array = new T[totalSize];
    }

    ArrayList(const T* const src, uint16_t len, uint8_t increaseStep): ArrayList(increaseStep) {
        IList<T>::add(src, len);
    }

    ArrayList(const T* const src, uint16_t len): ArrayList(src, len, 10) {}

    explicit ArrayList(): ArrayList(10){}

    uint16_t size() const override {
        return curIndex;
    }

    bool isEmpty() const override {
        return curIndex == 0;
    }

    T get(uint16_t index) const override {
        if(isEmpty()) return zeroValue;
        index = min(index, (uint16_t)(size() - 1));
        return array[index];
    }

    void set(uint16_t index, T value) {
        if(isEmpty()) return;
        index = min((uint16_t)(size() - 1), index);
        array[index] = value;
    };

    void add(uint16_t index, T value) override {
        index = min(size(), index);
        if(size() >= totalSize){
            extend();
        }
        insertElementInArray(array, totalSize, index, value);
        curIndex++;
    }

    void remove(uint16_t index, uint16_t quantity) override {
        if(index >= size()) return;
        quantity = min(quantity, (uint16_t)(size() - index));
        deleteElementsInArray(array, size(), index, quantity, zeroValue);
        curIndex -= quantity;
    }

    int32_t firstIndexOf(T value) const override {
        for(uint16_t i = 0; i < size(); i++){
            if(array[i] == value) return i;
        }
        return -1;
    }

    int32_t lastIndexOf(T value) const override {
        for(int32_t i = size() - 1; i >= 0; i--){
            if(array[i] == value) return i;
        }
        return -1;
    }

    void copyTo(T* dst, uint16_t start, uint16_t quantity) const override {
        copyPartOfArray(array, size(), start, quantity, dst);
    }


    void clear() override {
        curIndex = 0;
    }

    void fill(T value) override {
        fillArray(array, size(), value);
    }

    virtual ~ArrayList(){
        delete[] array;
    }

    void forEach(std::function<void(T)> lambda) const{
        for(uint16_t i = 0; i < size(); i++){
            lambda(array[i]);
        }
    }

    void forEach(std::function<void(T, uint16_t)> lambda) const{
        for(uint16_t i = 0; i < size(); i++){
            lambda(array[i], i);
        }
    }

    void forEachBreak(std::function<bool(T)> lambda) const{
        for(uint16_t i = 0; i < size(); i++){
            if(lambda(array[i])){
                break;
            }
        }
    }

    void forEachBreak(std::function<bool(T, uint16_t)> lambda) const{
        for(uint16_t i = 0; i < size(); i++){
            if(lambda(array[i], i)){
                break;
            }
        }
    }

    void forEachModify(std::function<T(T)> lambda){
        for(uint16_t i = 0; i < size(); i++){
            array[i] = lambda(array[i]);
        }
    }
};

#endif //ARRAY_LIST_H
