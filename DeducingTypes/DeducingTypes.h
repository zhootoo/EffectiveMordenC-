#pragma once
template <typename T>
void printTypeR(T& arg) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
template <typename T>
void printTypeCF(const T& arg) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
template <typename T>
void printType(T arg) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
template <typename T>
void printTypeP(T* arg) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

template <typename T>
void printTypeCP(const T* arg) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

template <typename T>
void printTypeRR(T&& arg) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}