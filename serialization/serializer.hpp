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


class serializable;


class  serializer 
{
/**************************** attributes **********************/
private:
    std::vector<uint8_t> data_;
    std::uint32_t shrink_count_;
    std::uint32_t buffer_shrink_threshold_;

public:
    /**************************** constructor **********************/
    
    serializer();

    serializer(std::uint32_t _buffer_shrink_threshold);


        
    /******************* fundemental functions **********************/

    bool serialize(const uint8_t _value);
    bool serialize(const uint16_t _value);
    bool serialize(const uint32_t _value, bool _omit_last_byte = false);
    bool serialize(const uint8_t *_data, uint32_t _length);
    bool serialize(const std::vector<uint8_t> &_data);
    // serialize any object that inherits serializable
    bool serialize(const serializable *_from);

    
    
    /***************************** getters **************************/

    virtual const uint8_t * get_data() const;
    virtual uint32_t get_capacity() const;
    virtual uint32_t get_size() const;  



    /***************************** setters **************************/

    virtual void set_data(uint8_t *_data, uint32_t _capacity);



    virtual void reset();
    
    void print();
    


    /*************************** deconstructor **********************/

    virtual ~serializer();
};






/**
 * Abstract base class for element that can be serialized.
 */
class serializable 
{
public:
    virtual ~serializable() {}

    /**
     * \brief serialize the content of the object
     */
    virtual bool serialize(serializer *_to) const = 0;
};


