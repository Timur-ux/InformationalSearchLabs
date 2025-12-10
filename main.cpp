#include "BPlusTree.hpp"
#include "Vector.hpp"
#include <cstring>
#include <ios>
#include <iostream>

struct People {
	std::string name;
	size_t age;

	People(std::string name, size_t age)
		: name(name), age(age) {
			std::cout << "People created"<< '\n';
		}
	People(const People &other)
		: name(other.name), age(other.age) {
		std::cout << "People COPY created"<< '\n';
	}
	People(People &&other) noexcept 
		:name(std::move(other.name)), age(other.age) {
		std::cout << "People MOVE created"<< '\n';
	}

	People& operator=(const People&other) {
		name = other.name;
		age = other.age;
		std::cout << "People COPY ASSIGN created"<< '\n';
		return *this;
	}
	People& operator=(People &&other) noexcept {
		if(this == &other) 
			return *this;
		name = std::move(other.name);
		age = other.age;
		std::cout << "People MOVE ASSIGN created"<< '\n';

		return *this;
	}
	~People() {
		std::cout << "People killed"<< '\n';
	}
};

int main() {
	// std::vector<People> v;
	IR::Vector<int> v;

	// People p[4] = {{"Edvard", 32}, {"Joe", 18}, {"Finn", 25}, {"Sam", 34}};
	// std::cout << "============"<< '\n';
	// v.push_back(p[0]);
	// std::cout << "============"<< '\n';
	// v.push_back(std::move(p[1]));
	// std::cout << "============"<< '\n';
	//
	// v[0] = p[2];
	// std::cout << "============"<< '\n';
	// v[1] = std::move(p[3]);
	// std::cout << "============"<< '\n';
}
