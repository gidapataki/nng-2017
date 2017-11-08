
#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <algorithm>
#include <stdexcept>

template<class T>
class matrix {
public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T*              iterator;
    typedef const T*        const_iterator;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
public:
    matrix();
    matrix(const matrix& o);
    matrix(size_type width, size_type height, const T& init_value = T());

    matrix& operator=(const matrix& o);

    virtual ~matrix();

    bool operator==(const matrix& o) const;
    bool operator!=(const matrix& o) const;

    reference at(size_type x, size_type y);
    const_reference at(size_type x, size_type y) const;

    reference operator()(size_type x, size_type y);
    const_reference operator()(size_type x, size_type y) const;

    void set_if_inbounds(size_type x, size_type y, const value_type& val);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    size_type width() const;
    size_type height() const;

    void swap(matrix<T>& rhs);
protected:
    size_type width_;
    size_type height_;
    T *data_;
};

template<class T>
matrix<T>::matrix() : width_(0), height_(0), data_(0) {}

template<class T>
matrix<T>::matrix(const matrix<T>& o) : width_(o.width_), height_(o.height_) {
    if ( o.data_ ) {
        data_ = new T[width_ * height_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
}

template<class T>
matrix<T>::matrix(size_type width, size_type height, const T& init_value) :
    width_(width),
    height_(height) {
    if ( width_ == 0 || height_ == 0 ) {
        data_ = 0;
    } else {
        data_ = new T[width_ * height_];
        std::fill(data_, data_ + (width_*height_), init_value);
    }
}

template<class T>
matrix<T>& matrix<T>::operator=(const matrix<T>& o) {
    if ( this == &o ) {
        return *this;
    }

    delete[] data_;

    width_ = o.width_;
    height_ = o.height_;
    if ( o.data_ ) {
        data_ = new T[width_ * height_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
    return *this;
}

template<class T>
matrix<T>::~matrix() {
    delete[] data_;
}

template<class T> inline
bool matrix<T>::operator==(const matrix<T>& o) const {
    return width_ == o.width_ && height_ == o.height && std::equal( data_, data_ + width_*height_, o.data_);
}

template<class T> inline
bool matrix<T>::operator!=(const matrix<T>& o) const {
    return !(*this == o);
}

template<class T> inline
typename matrix<T>::reference matrix<T>::at(size_type x, size_type y) {
    if ( x >= width_ || y >= height_ ) {
        throw std::out_of_range("matrix : Out of range");

    }
    return this->operator()(x,y);
}

template<class T> inline
typename matrix<T>::const_reference matrix<T>::at(size_type x, size_type y) const {
    
    if ( x >= width_ || y >= height_ ) {
        throw std::out_of_range("matrix : Out of range");
    }
    return this->operator()(x,y);
}

template<class T> inline
typename matrix<T>::reference matrix<T>::operator()(size_type x, size_type y) {
    return data_[y*width_+x];
}

template<class T> inline
typename matrix<T>::const_reference matrix<T>::operator()(size_type x, size_type y) const {
    return data_[y*width_+x];
}

template<class T> inline
void matrix<T>::set_if_inbounds(size_type x, size_type y, const value_type& val) {
    if ( x < width_ && y < height_ ) {
        this->operator()(x, y) = val;
    } else {
        //std::cout << x << ", " << y << std::endl;
    }
}

template<class T> inline
typename matrix<T>::iterator matrix<T>::begin() {
    return data_;
}

template<class T> inline
typename matrix<T>::iterator matrix<T>::end() {
    return data_ + (width_*height_);
}

template<class T> inline
typename matrix<T>::const_iterator matrix<T>::begin() const {
    return data_;
}

template<class T> inline
typename matrix<T>::const_iterator matrix<T>::end() const {
    return data_ + (width_*height_);
}

template<class T> inline
typename matrix<T>::size_type matrix<T>::width() const {
    return width_;
}

template<class T> inline
typename matrix<T>::size_type matrix<T>::height() const {
    return height_;
}

template<class T> inline
void matrix<T>::swap(matrix<T>& rhs) {
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(data_, rhs.data_);
}

#endif // MATRIX_HPP_
