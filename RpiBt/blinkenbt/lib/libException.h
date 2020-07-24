//
// Created by beton on 20.06.2020.
//

#ifndef PI2_LIBEXCEPTION_H
#define PI2_LIBEXCEPTION_H

#include <exception>

class fileDescriptorOpenException : public std::exception {};

class fileDescriptorWriteException : public std::exception {};

class socketOpenException : public std::exception {};

#endif //PI2_LIBEXCEPTION_H
