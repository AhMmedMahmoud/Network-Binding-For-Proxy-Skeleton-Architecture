#include <cstring>
#include "deserializer.hpp"

/**************************** constructor **********************/

deserializer::deserializer(std::uint32_t _buffer_shrink_threshold)
    : position_(data_.begin()),
      remaining_(0),
      buffer_shrink_threshold_(_buffer_shrink_threshold),
      shrink_count_(0) {
}

deserializer::deserializer(uint8_t *_data, std::size_t _length,
                           std::uint32_t _buffer_shrink_threshold)
    : data_(_data, _data + _length),
      position_(data_.begin()),
      remaining_(_length),
      buffer_shrink_threshold_(_buffer_shrink_threshold),
      shrink_count_(0) {
}

deserializer::deserializer(const deserializer &_other)
    : data_(_other.data_),
      position_(_other.position_),
      remaining_(_other.remaining_),
      buffer_shrink_threshold_(_other.buffer_shrink_threshold_),
      shrink_count_(_other.shrink_count_) {
}



/***************************** getters **************************/

std::size_t deserializer::get_available() const {
    return data_.size();
}

std::size_t deserializer::get_remaining() const {
    return remaining_;
}



/******************* fundemental functions **********************/

bool deserializer::deserialize(uint8_t& _value) {
    if (0 == remaining_)
        return false;

    _value = *position_++;

    remaining_--;
    return true;
}

bool deserializer::deserialize(uint16_t& _value) {
    if (2 > remaining_)
        return false;

    uint8_t byte0, byte1;
    byte0 = *position_++;
    byte1 = *position_++;
    remaining_ -= 2;

    _value = BYTES_TO_WORD(byte0, byte1);

    return true;
}

bool deserializer::deserialize(uint32_t &_value, bool _omit_last_byte) {
    if (3 > remaining_ || (!_omit_last_byte && 4 > remaining_))
        return false;

    uint8_t byte0 = 0, byte1, byte2, byte3;
    if (!_omit_last_byte) {
        byte0 = *position_++;
        remaining_--;
    }
    byte1 = *position_++;
    byte2 = *position_++;
    byte3 = *position_++;
    remaining_ -= 3;

    _value = BYTES_TO_LONG(
            byte0, byte1, byte2, byte3);

    return true;
}

bool deserializer::deserialize(uint8_t *_data, std::size_t _length) {
    if (_length > remaining_)
        return false;

    std::memcpy(_data, &data_[static_cast<std::vector<uint8_t>::size_type>(position_ - data_.begin())], _length);
    position_ += static_cast<std::vector<uint8_t>::difference_type>(_length);
    remaining_ -= _length;

    return true;
}

bool deserializer::deserialize(std::string &_target, std::size_t _length) {
    if (_length > remaining_ || _length > _target.capacity()) {
        return false;
    }
    _target.assign(position_, position_ + long(_length));
    position_ += long(_length);
    remaining_ -= _length;

    return true;
}

bool deserializer::deserialize(std::vector< uint8_t >& _value) {
    if (_value.capacity() > remaining_)
        return false;

    _value.assign(position_, position_
            + static_cast<std::vector<uint8_t>::difference_type>(_value.capacity()));
    position_ += static_cast<std::vector<uint8_t>::difference_type>(_value.capacity());
    remaining_ -= _value.capacity();

    return true;
}

bool deserializer::look_ahead(std::size_t _index, uint8_t &_value) const {
    if (_index > remaining_)
        return false;

    _value = *(position_ + static_cast<std::vector<uint8_t>::difference_type>(_index));

    return true;
}

bool deserializer::look_ahead(std::size_t _index, uint16_t &_value) const {
    if (_index+1 > remaining_)
        return false;

    std::vector< uint8_t >::iterator i = position_ +
            static_cast<std::vector<uint8_t>::difference_type>(_index);
    _value = BYTES_TO_WORD(*i, *(i+1));

    return true;
}

bool deserializer::look_ahead(std::size_t _index, uint32_t &_value) const {
    if (_index+3 > remaining_)
        return false;

    std::vector< uint8_t >::const_iterator i = position_ + static_cast<std::vector<uint8_t>::difference_type>(_index);
    _value = BYTES_TO_LONG(*i, *(i+1), *(i+2), *(i+3));

    return true;
}



/***************************** setters **************************/

void deserializer::set_data(const uint8_t *_data,  std::size_t _length) {
    if (0 != _data) {
        data_.assign(_data, _data + _length);
        position_ = data_.begin();
        remaining_ = static_cast<std::vector<uint8_t>::size_type>(data_.end() - position_);
    } else {
        data_.clear();
        position_ = data_.end();
        remaining_ = 0;
    }
}

void
deserializer::set_data(const std::vector<uint8_t> &_data) {

    data_ = std::move(_data);
    position_ = data_.begin();
    remaining_ = data_.size();
}

void deserializer::set_remaining(std::size_t _remaining) {
    remaining_ = _remaining;
}

void deserializer::append_data(const uint8_t *_data, std::size_t _length) {
    std::vector<uint8_t>::difference_type offset = (position_ - data_.begin());
    data_.insert(data_.end(), _data, _data + _length);
    position_ = data_.begin() + offset;
    remaining_ += _length;
}

void deserializer::drop_data(std::size_t _length) {
    if (position_ + static_cast<std::vector<uint8_t>::difference_type>(_length) < data_.end())
        position_ += static_cast<std::vector<uint8_t>::difference_type>(_length);
    else
        position_ = data_.end();
}

void deserializer::reset() {
    if (buffer_shrink_threshold_) {
        if (data_.size() < (data_.capacity() >> 1)) {
            shrink_count_++;
        } else {
            shrink_count_ = 0;
        }
    }
    data_.clear();
    position_ = data_.begin();
    remaining_ = data_.size();
    if (buffer_shrink_threshold_ && shrink_count_ > buffer_shrink_threshold_) {
        data_.shrink_to_fit();
        shrink_count_ = 0;
    }
}



/*************************** deconstructor **********************/

deserializer::~deserializer() {}