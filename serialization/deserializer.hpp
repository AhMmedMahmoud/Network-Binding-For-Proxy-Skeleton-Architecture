#include <iostream>
#include <string>
#include <vector>
#include <cstdint>


#define BYTES_TO_WORD(x0, x1) (uint16_t((x0) << 8 | (x1)))
#define BYTES_TO_LONG(x0, x1, x2, x3) (uint32_t((x0) << 24 | (x1) << 16 | (x2) << 8 | (x3)))
#define BYTES_TO_LONG_LONG(x0, x1, x2, x3, x4, x5, x6, x7) (uint64_t (x0) << 56 | uint64_t (x1) << 48 | uint64_t (x2) << 40 | uint64_t (x3) << 32 | uint64_t (x4) << 24 | uint64_t (x5) << 16 | uint64_t (x6) << 8 | uint64_t (x7))

#define WORDS_TO_LONG(x0, x1) (uint32_t((x0) << 16 | (x1)))

#define WORD_BYTE0(x) (uint8_t((x) & 0xFF))
#define WORD_BYTE1(x) (uint8_t((x) >> 8))

#define LONG_BYTE0(x) (uint8_t((x) & 0xFF))
#define LONG_BYTE1(x) (uint8_t(((x) >> 8) & 0xFF))
#define LONG_BYTE2(x) (uint8_t(((x) >> 16) & 0xFF))
#define LONG_BYTE3(x) (uint8_t(((x) >> 24) & 0xFF))

#define LONG_LONG_BYTE0(x) (uint8_t((x) & 0xFF))
#define LONG_LONG_BYTE1(x) (uint8_t(((x) >> 8) & 0xFF))
#define LONG_LONG_BYTE2(x) (uint8_t(((x) >> 16) & 0xFF))
#define LONG_LONG_BYTE3(x) (uint8_t(((x) >> 24) & 0xFF))
#define LONG_LONG_BYTE4(x) (uint8_t(((x) >> 32) & 0xFF))
#define LONG_LONG_BYTE5(x) (uint8_t(((x) >> 40) & 0xFF))
#define LONG_LONG_BYTE6(x) (uint8_t(((x) >> 48) & 0xFF))
#define LONG_LONG_BYTE7(x) (uint8_t(((x) >> 56) & 0xFF))

#define LONG_WORD0(x) (uint16_t((x) & 0xFFFF))
#define LONG_WORD1(x) (uint16_t(((x) >> 16) & 0xFFFF))




class deserializable;

class deserializer 
{
private:
    /**************************** attributes **********************/

    const std::uint32_t buffer_shrink_threshold_;
    std::uint32_t shrink_count_;

protected:
    std::vector<uint8_t> data_;
    std::vector<uint8_t>::iterator position_;
    std::size_t remaining_;

public:
    /**************************** constructor **********************/

    deserializer(std::uint32_t _buffer_shrink_threshold);
    deserializer(uint8_t *_data, std::size_t _length, std::uint32_t _buffer_shrink_threshold);
    deserializer(const deserializer& _other);



    /***************************** setters **************************/

    void set_data(const uint8_t *_data, std::size_t _length);
    void set_data(const std::vector<uint8_t> &_data);
    void set_remaining(std::size_t _remaining);
    void append_data(const uint8_t *_data, std::size_t _length);
    void drop_data(std::size_t _length);


    /***************************** getters **************************/

    std::size_t get_available() const;
    std::size_t get_remaining() const;




    /******************* fundemental functions **********************/

     // to be used (internally) by objects to deserialize their members
     // Note: this needs to be encapsulated!
    bool deserialize(uint8_t &_value);
    bool deserialize(uint16_t &_value);
    bool deserialize(uint32_t &_value, bool _omit_last_byte = false);
    bool deserialize(uint8_t *_data, std::size_t _length);
    bool deserialize(std::string &_target, std::size_t _length);
    bool deserialize(std::vector<uint8_t> &_value);

    bool look_ahead(std::size_t _index, uint8_t &_value) const;
    bool look_ahead(std::size_t _index, uint16_t &_value) const;
    bool look_ahead(std::size_t _index, uint32_t &_value) const;

    void reset();

    /*************************** deconstructor **********************/

    virtual ~deserializer();
};






class deserializable 
{
public:
    virtual ~deserializable() {
    }
    virtual bool deserialize(deserializer *_from) = 0;
};