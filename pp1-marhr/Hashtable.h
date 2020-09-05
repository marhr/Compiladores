// Mariana Hernandez

#include <iostream>
#include <stdio.h>
#include <string>
#include <queue>
using namespace std;

class Hashtable{
	public:
	Hashtable();
	Hashtable(string file);
	virtual ~Hashtable();

	string find(const string& key);
	void insert(const string& key, const string& value);
	string remove(const string& key);
	void print();
	
	protected:
	unsigned int hash(const string& key);
	int find_index(const string& key, bool override_duplicated_key);
	
	const static unsigned int SIZE = 100;
	string keys[SIZE];
	string values[SIZE];
};