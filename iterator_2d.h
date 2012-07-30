#ifndef ITERATOR_2D_H
#define ITERATOR_2D_H

#include <array>
#include <iterator>
#include <exception>

template<typename T, std::size_t N, size_t M>
class iterator_2d : public std::iterator<std::bidirectional_iterator_tag, T> {

	std::array<std::array<T, N>, M>& arr;
	size_t x, y;

public:
	iterator_2d(std::array<std::array<T, N>, M>& _arr, size_t _x = 0, size_t _y = 0);
	iterator_2d(const iterator_2d& rhs);
	iterator_2d& operator=(const iterator_2d& rhs);
    T& operator*();
    T* operator->();
	iterator_2d& operator++();
	iterator_2d operator++(int);
	iterator_2d& operator--();
	iterator_2d operator--(int);
	bool equal(iterator_2d const& rhs) const;

	std::pair<int, int> loc() const;
};

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M>::iterator_2d(std::array<std::array<T, N>, M>& _arr, size_t _x, size_t _y) : arr(_arr), x(_x), y(_y) {
	if (x < 0 || y < 0 || y > M || (y < M-1 && x >= N) || (y == M-1 && x > N)) 
		throw std::exception();
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M>::iterator_2d(const iterator_2d<T, N, M>& rhs) : arr(rhs.arr) {
	*this = rhs;
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M>& iterator_2d<T, N, M>::operator=(const iterator_2d<T, N, M>& rhs) {
	arr = rhs.arr;
	x = rhs.x;
	y = rhs.y;
	return *this;
}

template<typename T, size_t N, size_t M>
T& iterator_2d<T, N, M>::operator*() {
	return arr[x][y];
}

template<typename T, size_t N, size_t M>
T* iterator_2d<T, N, M>::operator->() {
	return arr[x][y];
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M>& iterator_2d<T, N, M>::operator++() {
	if (x == N && y==M-1) return *this;

	x++;

	if (y == M-1) return *this;

	if (x == N) {
		x = 0; y++;
	}

	return *this;
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> iterator_2d<T, N, M>::operator++(int) {
	iterator_2d<T, N, M> result(*this);
	++(*this);
	return result;
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M>& iterator_2d<T, N, M>::operator--() {
	x--;
	if (x < 0) {
		x = N-1; y--;
		if (y < 0) { x = 0; y = 0; }
	}

	return *this;
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> iterator_2d<T, N, M>::operator--(int) {
	iterator_2d<T, N, M> result(*this);
	++(*this);
	return result;
}

template<typename T, size_t N, size_t M>
bool iterator_2d<T, N, M>::equal(iterator_2d const& rhs) const {
	return x == rhs.x && y == rhs.y;
}

template<typename T, size_t N, size_t M>
std::pair<int, int> iterator_2d<T, N, M>::loc() const {
	return std::make_pair(x, y);
}

template<typename T, size_t N, size_t M>
bool operator==(const iterator_2d<T, N, M>& a, const iterator_2d<T, N, M>& b) {
	return a.equal(b);
}

template<typename T, size_t N, size_t M>
bool operator!=(const iterator_2d<T, N, M>& a, const iterator_2d<T, N, M>& b) {
	return !a.equal(b);
}

//Iterator creators

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> begin(std::array<std::array<T,  N>, M>& arr) {
	return iterator_2d<T, N, M>(arr);
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> end(std::array<std::array<T,  N>, M>& arr) {
	return iterator_2d<T, N, M>(arr, N, M-1);
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> beginq(std::array<std::array<T,  N>, M>& arr) {
	return iterator_2d<T, N, M>(arr);
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> endq(std::array<std::array<T,  N>, M>& arr) {
	return iterator_2d<T, N, M>(arr, N, M-1);
}

template<typename T, size_t N, size_t M>
iterator_2d<T, N, M> iterator_at(std::array<std::array<T, N>, M>& arr, size_t x, size_t y) {
	return iterator_2d<T, N, M>(arr, x, y);
}

template<typename T, size_t N, size_t M>
std::reverse_iterator<iterator_2d<T, N, M>> rbegin(std::array<std::array<T,  N>, M>& arr) {
	return std::reverse_iterator<iterator_2d<T,N,M>>(end(arr));
}

template<typename T, size_t N, size_t M>
std::reverse_iterator<iterator_2d<T, N, M>> rend(std::array<std::array<T,  N>, M>& arr) {
	return std::reverse_iterator<iterator_2d<T,N,M>>(begin(arr));
}

template<typename T, size_t N, size_t M>
std::reverse_iterator<iterator_2d<T, N, M>> r_iterator_at(std::array<std::array<T,  N>, M>& arr, size_t x, size_t y) {
	return std::reverse_iterator<iterator_2d<T,N,M>>(++iterator_at(arr, x, y));
}

#endif
