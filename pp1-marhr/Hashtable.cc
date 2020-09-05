// Mariana Hernandez

#include "Hashtable.h"

Hashtable::Hashtable() {
	for (int i = 0; i < SIZE; i++) {
		keys[i] = string();
		values[i] = string();
	}
}

Hashtable::~Hashtable()
{

}

unsigned int Hashtable::hash(const string& key){
	unsigned int value = 0 ;
	for (int i = 0; i < key.length(); i++){
		value = 37*value + key[i];
	}
	return value;
}

void Hashtable::print() {
	cout << "{";
	for (int i = 0; i < SIZE; i++){
		if (!keys[i].empty()){
			cout << keys[i] << ":" << values[i] << ", ";
		}
	}
	cout << "}" << endl;
}

int Hashtable::find_index(const string& key, bool override_duplicate_key = true) {
	unsigned int h = hash(key) % SIZE, offset = 0, index;  
	while (offset < SIZE) {
		index = (h + offset) % SIZE;

		if (keys[index].empty() || (override_duplicate_key && keys[index] == key))
			return index;
		offset++;
	}
	return -1;
}

void Hashtable::insert(const string& key, const string& value) {
	int index = find_index(key);
	if (index == -1) {
		cout << "is full!" << endl;
		return;
	}

	keys[index] = key;
	values[index] = value;
}

string Hashtable::find(const string& key) {
	// printf("%s:%d\n", __PRETTY_FUNCTION__,__LINE__);
	int index = find_index(key);
	if (index != -1)
		return values[index];

	return "";
}

string Hashtable::remove(const string& key) {
	int index = find_index(key);
	if (index == -1) 
		return "";

	string value = values[index];
	keys[index].clear();
	values[index].clear();

	return value;
}