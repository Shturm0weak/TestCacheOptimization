#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "Timer.h"
#include <map>

#define MAX_PREALLOCATED_INSTANCES 10000

double debug::Timer::s_End = 0;

namespace memory {
	template<class T>
	std::map<char*, uint32_t>::iterator PreAllocateMemory(std::map<char*, uint32_t>& memoryPool) {
		if (memoryPool.size() == 0) {
			char* newPreAllocMemory = new char[MAX_PREALLOCATED_INSTANCES * sizeof(T)];
			memoryPool.insert(std::make_pair(newPreAllocMemory, 0));
		}
		char* memoryPtr = memoryPool.rbegin()->first;
		auto iter = memoryPool.find(memoryPtr);
		if (iter->second == MAX_PREALLOCATED_INSTANCES) {
			char* newPreAllocMemory = new char[MAX_PREALLOCATED_INSTANCES * sizeof(T)];
			iter = memoryPool.insert(std::make_pair(newPreAllocMemory, 0)).first;
			memoryPtr = newPreAllocMemory;
		}
		return iter;
	}
}

class Component {
public:

	static std::vector<Component*> s_Components;
	static std::map<char*, uint32_t> s_ComponentMemoryPool;
	char* m_MemoryPoolPtr = nullptr;

	uint64_t m_Id = 0;
	uint32_t m_Type = 0;
	float x = 0, y = 0, z = 0;

	template <typename T>
	static T* Create() {
		auto iter = memory::PreAllocateMemory<T>(s_ComponentMemoryPool);
		T* component = new(iter->first + iter->second * sizeof(T)) T();
		component->m_Id = s_Components.size() - 1;
		component->m_Type = sizeof(T);
		component->m_MemoryPoolPtr = iter->first;
		iter->second++;
		return component;
	}

	//Just a function to take processor time
	virtual float F() { return 0.0f; }

	Component() {
		s_Components.push_back(this);
	}

	virtual ~Component() {}
};

class Transform : public Component {
public:
	std::string trasnform;

	float F() {
		x = m_Id;
		y = x * x;
		z = std::sqrtf(x * x);
		return std::sqrtf(x * x + y * y + z * z);
	}
};

std::map<char*, uint32_t> Component::s_ComponentMemoryPool;
std::vector<Component*> Component::s_Components;

#define NUM_OF_TESTS 100
#define NUM_OF_COMPONENTS 100000

int main() {
	double time = 0;
	std::cin.get();
	
	for (uint32_t i = 0; i < NUM_OF_COMPONENTS; i++)
	{
		new Transform();
	}
	std::cout << "New entities: " << Component::s_Components.size() << std::endl;
	for (uint32_t j = 0; j < NUM_OF_TESTS; j++)
	{
		time += debug::Timer::s_End;
		debug::Timer t;
		for (uint32_t i = 0; i < Component::s_Components.size(); i++)
		{
			Component::s_Components[i]->F();
		}
	}
	std::cout << time / NUM_OF_TESTS << std::endl;
	time = 0;
	for (uint32_t i = 0; i < NUM_OF_COMPONENTS; i++)
	{
		delete Component::s_Components[i];
	}
	Component::s_Components.clear();


	for (uint32_t i = 0; i < NUM_OF_COMPONENTS; i++)
	{
		Component::Create<Transform>();
	}
	std::cout << "Cached entities: " << Component::s_Components.size() << std::endl;
	for (uint32_t j = 0; j < NUM_OF_TESTS; j++)
	{	
		time += debug::Timer::s_End;
		debug::Timer t;
		for (uint32_t i = 0; i < Component::s_Components.size(); i++)
		{
			Component::s_Components[i]->F();
		}
	}
	for (auto iter : Component::s_ComponentMemoryPool)
	{
		delete iter.first;
	}
	std::cout << time / NUM_OF_TESTS << std::endl;
}