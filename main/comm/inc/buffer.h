#ifndef BUFFER_H
#define BUFFER_H
#include <cstdint>
#include "array_fun.h"
#include "math_fun.h"

template <typename T>
T getX(uint16_t index, uint8_t *const bytes, uint16_t len){
    T result = 0;
    uint8_t size = sizeof(T);
    if(index >= len) return 0;
    for(uint16_t i = 0; i < size; i++){
        if(i + index >= len) break;
        result |= (bytes[i + index] << (8 * (size - i - 1)));
    }
    return result;
}

class InternalBuffer{
private:
    uint16_t bufferSize{0};
    uint16_t bufferIndex{0};
    void limIndex(uint16_t& index) const;
    void limQuan(uint16_t& quantity, uint16_t shift) const;
    void clearXBytes(uint16_t start, uint16_t quantity, uint8_t size);
protected:
    uint8_t* buffer{nullptr};
    //add
    virtual void addByte(uint8_t value);
    virtual void addByte(const uint8_t *const src, uint16_t start, uint16_t quantity);
    virtual void addByte(const uint8_t *const src, uint16_t len);
public:
    explicit InternalBuffer(uint16_t bufferSize);
    ~InternalBuffer();
    //buffer info
    bool bufferIsOverFlow() const;
    bool bufferIsEmpty() const;
    bool bufferIsNotEmpty() const;
    uint16_t getBufferSize() const;
    uint16_t getBufferIndex() const;
    //get
    uint8_t getByte(uint16_t index);
    uint8_t getByte();
    void getByte(uint8_t *const dst, uint16_t start, uint16_t quantity);
    uint16_t getWord(uint16_t index);
    uint16_t getWord();
    void getWord(uint16_t *const dst, uint16_t start, uint16_t quantity);
    uint32_t getDWord(uint16_t index);
    uint32_t getDWord();
    void getDWord(uint32_t *const dst, uint16_t start, uint16_t quantity);
    float getFloat(uint16_t index);
    float getFloat();
    void getFloat(float *const dst, uint16_t start, uint16_t quantity);
    //clear
    void clearByte(uint16_t start, uint16_t quantity);
    void clearByte(uint16_t index);
    void clearByte();
    void clearWord(uint16_t start, uint16_t quantity);
    void clearWord(uint16_t index);
    void clearWord();
    void clearDWord(uint16_t start, uint16_t quantity);
    void clearDWord(uint16_t index);
    void clearDWord();
    void clearFloat(uint16_t start, uint16_t quantity);
    void clearFloat(uint16_t index);
    void clearFloat();
    //getAndClear
    uint8_t getAndClearByte(uint16_t index);
    uint8_t getAndClearByte();
    void getAndClearByte(uint8_t *const dst, uint16_t start, uint16_t quantity);
    uint16_t getAndClearWord(uint16_t index);
    uint16_t getAndClearWord();
    void getAndClearWord(uint16_t *dst, uint16_t start, uint16_t quantity);
    uint32_t getAndClearDWord(uint16_t index);
    uint32_t getAndClearDWord();
    void getAndClearDWord(uint32_t *const dst, uint16_t start, uint16_t quantity);
    float getAndClearFloat(uint16_t index);
    float getAndClearFloat();
    void getAndClearFloat(float *const dst, uint16_t start, uint16_t quantity);
    //all
    void getAll(uint8_t *const dst);
    void clearALl();
};

class Buffer: public InternalBuffer{
private:
    void set(uint8_t len, uint16_t index, uint32_t value);
public:
    explicit Buffer(uint16_t bufferSize);
    //add
    void addByte(uint8_t value) override;
    void addByte(const uint8_t *src, uint16_t start, uint16_t quantity) override;
    void addByte(const uint8_t *src, uint16_t len) override;

    void addWord(uint16_t value);
    void addWord(const uint16_t *src, uint16_t start, uint16_t quantity);
    void addWord(const uint16_t *src, uint16_t len);

    void addDWord(uint32_t value);
    void addDWord(const uint32_t *src, uint16_t start, uint16_t quantity);
    void addDWord(const uint32_t *src, uint16_t len);

    void addFloat(float value);
    void addFloat(const float *src, uint16_t start, uint16_t quantity);
    void addFloat(const float *src, uint16_t len);
    //insert
    void insertByte(uint16_t index, uint8_t value);
    void insertByte(uint8_t value);
    void insertByte(uint16_t index, const uint8_t *src, uint16_t start, uint16_t quantity);
    void insertByte(const uint8_t *src, uint16_t start, uint16_t quantity);
    void insertByte(uint16_t index, const uint8_t *src, uint16_t len);
    void insertByte(const uint8_t *src, uint16_t len);

    void insertWord(uint16_t index, uint16_t value);
    void insertWord(uint16_t value);
    void insertWord(uint16_t index, const uint16_t *src, uint16_t start, uint16_t quantity);
    void insertWord(const uint16_t *src, uint16_t start, uint16_t quantity);
    void insertWord(uint16_t index, const uint16_t *src, uint16_t len);
    void insertWord(const uint16_t *src, uint16_t len);

    void insertDWord(uint16_t index, uint32_t value);
    void insertDWord(uint32_t value);
    void insertDWord(uint16_t index, const uint32_t *src, uint16_t start, uint16_t quantity);
    void insertDWord(const uint32_t *src, uint16_t start, uint16_t quantity);
    void insertDWord(uint16_t index, const uint32_t *src, uint16_t len);
    void insertDWord(const uint32_t *src, uint16_t len);

    void insertFloat(uint16_t index, float value);
    void insertFloat(float value);
    void insertFloat(uint16_t index, const float *src, uint16_t start, uint16_t quantity);
    void insertFloat(const float *src, uint16_t start, uint16_t quantity);
    void insertFloat(uint16_t index, const float *src, uint16_t len);
    void insertFloat(const float *src, uint16_t len);
    //set
    void setByte(uint16_t index, uint8_t value);
    void setWord(uint16_t index, uint16_t value);
    void setDWord(uint16_t index, uint32_t value);
    void setFloat(uint16_t index, float value);
};

#endif //BUFFER_H
