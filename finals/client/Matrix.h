#pragma once
#include <algorithm>
#include <stdexcept>


template<class T>
class Matrix {
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
    Matrix();
    Matrix(const Matrix& o);
    Matrix(size_type width, size_type height, const T& init_value = T());

    Matrix& operator=(const Matrix& o);

    virtual ~Matrix();

    bool operator==(const Matrix& o) const;
    bool operator!=(const Matrix& o) const;

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

    void swap(Matrix<T>& rhs);
protected:
    size_type width_;
    size_type height_;
    T *data_;
};

template<class T>
Matrix<T>::Matrix() : width_(0), height_(0), data_(0) {}

template<class T>
Matrix<T>::Matrix(const Matrix<T>& o) : width_(o.width_), height_(o.height_) {
    if ( o.data_ ) {
        data_ = new T[width_ * height_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
}

template<class T>
Matrix<T>::Matrix(size_type width, size_type height, const T& init_value) :
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
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& o) {
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
Matrix<T>::~Matrix() {
    delete[] data_;
}

template<class T> inline
bool Matrix<T>::operator==(const Matrix<T>& o) const {
    return width_ == o.width_ && height_ == o.height && std::equal( data_, data_ + width_*height_, o.data_);
}

template<class T> inline
bool Matrix<T>::operator!=(const Matrix<T>& o) const {
    return !(*this == o);
}

template<class T> inline
typename Matrix<T>::reference Matrix<T>::at(size_type x, size_type y) {
    if ( x >= width_ || y >= height_ ) {
        throw std::out_of_range("Matrix : Out of range");

    }
    return this->operator()(x,y);
}

template<class T> inline
typename Matrix<T>::const_reference Matrix<T>::at(size_type x, size_type y) const {
    if ( x >= width_ || y >= height_ ) {
        throw std::out_of_range("Matrix : Out of range");
    }
    return this->operator()(x,y);
}

template<class T> inline
typename Matrix<T>::reference Matrix<T>::operator()(size_type x, size_type y) {
    return data_[y*width_+x];
}

template<class T> inline
typename Matrix<T>::const_reference Matrix<T>::operator()(size_type x, size_type y) const {
    return data_[y*width_+x];
}

template<class T> inline
void Matrix<T>::set_if_inbounds(size_type x, size_type y, const value_type& val) {
    if ( x < width_ && y < height_ ) {
        this->operator()(x, y) = val;
    } else {
        //std::cout << x << ", " << y << std::endl;
    }
}

template<class T> inline
typename Matrix<T>::iterator Matrix<T>::begin() {
    return data_;
}

template<class T> inline
typename Matrix<T>::iterator Matrix<T>::end() {
    return data_ + (width_*height_);
}

template<class T> inline
typename Matrix<T>::const_iterator Matrix<T>::begin() const {
    return data_;
}

template<class T> inline
typename Matrix<T>::const_iterator Matrix<T>::end() const {
    return data_ + (width_*height_);
}

template<class T> inline
typename Matrix<T>::size_type Matrix<T>::width() const {
    return width_;
}

template<class T> inline
typename Matrix<T>::size_type Matrix<T>::height() const {
    return height_;
}

template<class T> inline
void Matrix<T>::swap(Matrix<T>& rhs) {
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(data_, rhs.data_);
}
