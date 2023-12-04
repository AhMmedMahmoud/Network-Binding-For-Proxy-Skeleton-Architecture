#include <iostream>
#include "serializer.hpp"


/**************************** constructor **********************/

serializer::serializer() :
        data_(0),
        shrink_count_(0),
        buffer_shrink_threshold_(0) {
}

serializer::serializer(std::uint32_t _buffer_shrink_threshold) :
        data_(0),
        shrink_count_(0),
        buffer_shrink_threshold_(_buffer_shrink_threshold) {
}



/******************* fundemental functions **********************/

bool serializer::serialize(const uint8_t _value) 
{
    data_.push_back(_value);
    return true;
}

bool serializer::serialize(const uint16_t _value) 
{
    data_.push_back(WORD_BYTE1(_value));
    data_.push_back(WORD_BYTE0(_value));
    return true;
}

bool serializer::serialize(const uint32_t _value, bool _omit_last_byte) 
{
    if (!_omit_last_byte) 
    {
        data_.push_back(LONG_BYTE3(_value));
    }
    data_.push_back(LONG_BYTE2(_value));
    data_.push_back(LONG_BYTE1(_value));
    data_.push_back(LONG_BYTE0(_value));
    return true;
}

bool serializer::serialize(const uint8_t *_data, uint32_t _length) {
    try {
        data_.insert(data_.end(), _data, _data + _length);
    } catch(const std::bad_alloc &e) {
        std::cout << "Couldn't allocate memory in serializer::serialize(*_data, length)" << e.what();
        return false;
    }
    return true;
}


/*
std::vector<uint8_t> data_ = {1, 2, 3};  // Initial content of data_ vector
std::vector<uint8_t> _data = {4, 5, 6};  // Content of _data vector to be inserted into data_
data_.insert(data_.end(), _data.begin(), _data.end());

After this operation, the `data_` vector will contain the following elements:
1, 2, 3, 4, 5, 6

*/
bool serializer::serialize(const std::vector<uint8_t> &_data) 
{
    try 
    {
        data_.insert(data_.end(),_data.begin(), _data.end());
    } 
    catch(const std::bad_alloc &e) {
        std::cout << "Couldn't allocate memory in serializer::serialize(vector)" << e.what();
        return false;
    }
    return true;
}


/***************************** getters **************************/

const uint8_t * serializer::get_data() const 
{
    return data_.data();
}

uint32_t serializer::get_capacity() const 
{
    return static_cast<std::uint32_t>(data_.max_size());
}

uint32_t serializer::get_size() const 
{
    return static_cast<std::uint32_t>(data_.size());
}




/***************************** setters **************************/

void serializer::set_data(uint8_t *_data, uint32_t _capacity) 
{
    data_.clear();
    try {
        data_.insert(data_.end(), _data, _data + _capacity);
    } catch(const std::bad_alloc &e) {
        std::cout << "Couldn't allocate memory in serializer::set_data" << e.what();
    }
}





bool serializer::serialize(const serializable *_from) {
    return (_from && _from->serialize(this));
}

void serializer::reset()
{
    if (buffer_shrink_threshold_) {
        if (data_.size() < (data_.capacity() >> 1)) {
            shrink_count_++;
        } else {
            shrink_count_ = 0;
        }
    }
    data_.clear();
    if (buffer_shrink_threshold_ && shrink_count_ > buffer_shrink_threshold_) {
        data_.shrink_to_fit();
        shrink_count_ = 0;
    }
}


void serializer::print()
{
    std::cout << "--- vector ---\n";
    for (uint8_t value : data_) {
        std::cout << static_cast<int>(value) << std::endl;
    }
    std::cout << std::endl;
}



/*************************** deconstructor **********************/

serializer::~serializer() {}

