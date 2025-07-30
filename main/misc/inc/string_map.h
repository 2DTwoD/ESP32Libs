#ifndef STRING_MAP_H
#define STRING_MAP_H
#include "array_list.h"

template<typename T>
struct StringMapIndex{
    char *key;
    T value;
    StringMapIndex(const char *key, T value) :key(new char[strlen(key) + 1]), value(value) {
        strcpy(this->key, key);
    }

    virtual ~StringMapIndex() {
        delete[] key;
    }
};

template<typename T>
struct SMIPair{
    StringMapIndex<T> *stringMapIndex;
    uint16_t num;
};

template<typename T>
class StringMap{
private:
    T zeroValue;
    ArrayList<StringMapIndex<T>*> smiList;

    SMIPair<T> find(const char *name){
        SMIPair<T> result{nullptr, 0};
        smiList.forEachBreak([&name, &result](auto smi, uint16_t index){
           if(strcmp(smi->key, name) == 0){
               result = {smi, index};
               return true;
           }
           return false;
        });
        return result;
    }
public:
    explicit StringMap(T zeroValue): zeroValue(zeroValue), smiList(nullptr) {}

    ~StringMap() {
        clear();
    }

    void add(const char *name, T value){
        auto smiPair = find(name);
        if(smiPair.stringMapIndex == nullptr){
            smiList.add(new StringMapIndex<T>{name, value});
        }
    }

    void remove(const char *name){
        auto smiPair = find(name);
        if(smiPair.stringMapIndex != nullptr){
            smiList.remove(smiPair.num);
            delete smiPair.stringMapIndex;
        }
    }

    void clear(){
        smiList.forEach([](auto smi){
            delete smi;
        });
        smiList.clear();
    }

    T get(const char *name){
        auto smiPair = find(name);
        if(smiPair.stringMapIndex != nullptr){
            return smiPair.stringMapIndex->value;
        }
        return zeroValue;
    }

    void set(const char *name, T value){
        auto smiPair = find(name);
        if(smiPair.stringMapIndex != nullptr){
            smiPair.stringMapIndex->value = value;
        }
    }

    bool isExist(const char *name){
        return find(name).stringMapIndex != nullptr;
    }

    uint16_t size(){
        return smiList.size();
    }

    void forEach(std::function<void(char*, T)> lambda) const{
        smiList.forEach([&lambda](StringMapIndex<T>* smi){
            lambda(smi->key, smi->value);
        });
    }

    void forEachBreak(std::function<bool(char*, T)> lambda) const{
        smiList.forEachBreak([&lambda](StringMapIndex<T>* smi){
            return lambda(smi->key, smi->value);
        });
    }
};
#endif //STRING_MAP_H
