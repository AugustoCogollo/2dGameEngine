#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

const unsigned int MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> Signature;



class Entity {
private:
	int id;

public:
	Entity(int id) : id(id){};
	Entity(const Entity& entity) = default;
	int GetId() const;

	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
};

struct IComponent {
	static int nextId;
};

template <typename T>
class Component : public IComponent {
	 
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};

class System {
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	template <typename TComponent> void RequireComponent();
};

class IPool {
public: 
	virtual ~IPool() {};
};

template <typename T>
class Pool : public IPool {
private:
	std::vector<T> data;

public:
	Pool(int size = 100) { data.resize(size); }
	virtual ~Pool() = default;

	bool isEmpty() const {
		return data.empty();
	}

	int GetSize() const {
		return data.size();
	}

	void Resize(int n) {
		data.resize(n);
	}

	void Clear() {
		data.clear();
	}

	void Add(T object) {
		data.push_back(object);
	}

	void Set(int index, T object) {
		data[index] = object;
	}

	T& Get(int index) {
		return static_cast<T&>(data[index]);
	}

	T& operator [](unsigned int index) {
		return data[index];
	}
};

class Registry {
private:
	int numEntities = 0;
	
	//Vector index = component type id
	//Pool index = entity id
	std::vector<IPool*> componentPools;

	//Vector of component signatures per entity, saying which component is turned on
	//Vector index = entity id
	std::vector<Signature> entityComponentSignatures;

	std::unordered_map<std::type_index, System*> systems;

	//Set of entities flagged to be added or removed in the next frame
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeRenoved;

public:
	Registry() = default;

	void Update();
	
	Entity CreateEntity();

	//Component management
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename TComponent> void RemoveComponent(Entity entity);
	template <typename TComponent> bool HasComponent(Entity entity) const;

	//System management
	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename TSystem> void RemoveSystem();
	template <typename TSystem> bool HasSystem() const;
	template <typename TSystem> TSystem& GetSystem() const;

	//Checks the component signature of an entity and adds the entity to the systems that are interested in it
	void AddEntityToSystem(Entity entity);
};

template <typename TComponent>
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
	
}

//Component templates
template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity,TArgs&& ...args) {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		Pool<TComponent>* newComponentPool = new Pool<TComponent>();
		componentPools[componentId] = newComponentPool;
	}

	Pool<TComponent>* componentPool = componentPools[componentId];

	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);

	entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();

	entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();
	
	return entityComponentSignatures[entityId].test(componentId);
}


//System templates
template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
	TSystem* newSystem(new TSystem(std::forward<TArgs>(args)...));
	systems.insert(std::make_pair(std::type_index(typeid(TSystem()), newSystem)));
}

template <typename TSystem> 
void Registry::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

#endif 