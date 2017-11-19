///file:table.c
///description:a generic hash table implementation
///@author avv8047 : Azhur Viano


#include "table.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


/// Create a new hash table.
/// @param hash The key's hash function
/// @param equals The key's equal function for comparison
/// @param print A print function for the key/value, used for dump debugging
/// @exception Assert fails if can't allocate space
/// @return A newly created table
Table* create(long (*hash)(void* key),
	      bool (*equals)(void* key1, void* key2),
	      void (*print)(void* key1, void* key2)){
  
     Table* new_table = (Table*) malloc(sizeof(Table));
  
     if(new_table == NULL){
	  fprintf(stderr, "table::create() failed to allocate space\n");
	  assert(NULL);
     }
  
     new_table->table = (Entry**) calloc(INITIAL_CAPACITY, sizeof(Entry*));
  
     if(new_table->table == NULL){
	  fprintf(stderr, "table::create() failed to allocate space\n");
	  assert(NULL);
     }
  
     new_table->size = 0;
     new_table->capacity = INITIAL_CAPACITY;
  
     new_table->hash = hash;
     new_table->equals = equals;
     new_table->print = print;
  
     new_table->collisions = 0;
     new_table->rehashes = 0;

     return new_table;
}


/// Destroy a table
/// @param t The table to destroy
void destroy(Table* t){
     Entry** entries = t->table;
     int entries_size = t->capacity;
     int i;

     for(i = 0; i < entries_size; i++){
	  if(*(entries + i) != NULL){
	       free(*(entries + i));
	  }
     }

     free(entries);
     free(t);

     return;
}


/// Print out information about the hash table (size,
/// capacity, collisions, rehashes).  If full is
/// true, it will also print out the entire contents of the hash table,
/// using the registered print function with each non-null entry.
/// @param t The table to display
/// @param full Do a full dump of entire table contents
void dump(Table* t, bool full){
     printf("Size: %lu\n", t->size);
     printf("Capacity: %lu\n", t->capacity);
     printf("Collisions: %lu\n", t->collisions);
     printf("Rehashes: %lu\n", t->rehashes);

     if(full){
	  int i;
	  int capac = t->capacity;
	  Entry** entries = t->table;
	  Entry* entry = NULL;

	  for(i = 0; i < capac; i++){
	       entry = *(entries + i);
	       printf("%d: ", i);
	       if(entry == NULL){
		    printf("null");
	       }
	       else{
		    printf("(");
		    t->print(entry->key, entry->value);
		    printf(")");
	       }
	       printf("\n");
	  }
     }
}


/// Get the value associated with a key from the table.  This function
/// uses the registered hash function to locate the key, and the
/// registered equals function to check for equality.
/// @pre The table must have the key, or else it will assert fail
/// @param t The table
/// @param key The key
/// @return The associated value of the key
void* get(Table* t, void* key){
     size_t i = t->hash(key) % t->capacity;
     Entry** entries = t->table;
     Entry* entry = *(entries + i);

     while(entry != NULL){
	  if(t->equals(entry->key, key)){
	       return entry->value;
	  }
	  i++;
	  t->collisions += 1;
	  if(i == t->capacity){
	       i = 0;
	  }
	  entry = *(entries + i);
     }

     return NULL;
}


/// Check if the table has a key.  This function uses the registered hash
/// function to locate the key, and the registered equals function to
/// check for equality.
/// @param t The table
/// @param key The key
/// @return Whether the key exists in the table.
bool has(Table* t, void* key){
     Entry** entries = t->table;
     size_t i = t->hash(key) % t->capacity;
     Entry* entry = *(entries + i);
     size_t searched = 0;
     while(entry != NULL){
	  if(t->equals(entry->key, key)){
	       return true;
	  }
	  i++;
	  t->collisions += 1;
	  searched++;
	  if(searched == t->capacity){
	       t->collisions -= searched;
	       return false;
	  }
	  if(i == t->capacity){
	       i = 0;
	  }
	  entry = *(entries + i);
     }

     return false;
}


/// Get the collection of keys from the table.  This function allocates
/// space to store the keys, which the caller is responsible for freeing.
/// @param t The table
/// @exception Assert fails if can't allocate space
/// @return A dynamic array of keys
void** keys(Table* t){
     void** key_list = calloc(t->size, sizeof(void*));

     if(key_list == NULL){
	  fprintf(stderr, "table::keys() failed to allocate memory\n");
	  assert(NULL);
     }
  
     size_t i, j;
     Entry** entries = t->table;
     Entry* entry = *(entries);

     for(i = 0, j = 0; j < t->size;){
	  if(entry != NULL){
	       *(key_list + j) = entry->key;
	       j++;
	  }
	  i++;
	  if(i < t->capacity){
	       entry = *(entries + i);
	  }
     }
     return key_list;
}


///Resize the table and rehash all of its keys
///@param t a pointer to the table to be resized
static void rehash(Table* t){
     void** key_list = keys(t);
     void** val_list = values(t);
     size_t n_elements = t->size;
     size_t i;

     for(i = 0; i < t->capacity; i++){
	  if(*(t->table + i) != NULL){
	       free(*(t->table + i));
	  }
     }
  
     t->capacity *= RESIZE_FACTOR;
     t->size = 0;
     t->rehashes += 1;


     free(t->table);
     t->table = (Entry**) calloc(t->capacity, sizeof(Entry*));

     if(t->table == NULL){
	  fprintf(stderr, "table::rehash() failed to allocate memory\n");
	  assert(NULL);
     }


     for(i = 0; i < n_elements; i++){
	  put(t, *(key_list + i), *(val_list + i));
     }

     free(key_list);
     free(val_list);

     return;
}


/// Add a key value pair to the table, or update an existing key's value.
/// This function uses the registered hash function to locate the key,
/// and the registered equals function to check for equality.
/// @param t The table
/// @param key The key
/// @param value The value
/// @exception Assert fails if can't allocate space
/// @return The old value in the table, if one exists.
void* put(Table* t, void* key, void* value){
     if((double) t->size / (double) t->capacity >= LOAD_THRESHOLD){
	  rehash(t);
     }
  
     size_t i = t->hash(key) % t->capacity;
     Entry** entries = t->table;
     Entry* entry = *(entries + i);
  
     while(entry != NULL){
	  if(t->equals(entry->key, key)){
	       void* old = entry->value;;
	       entry->value = value;
	       return old;
	  }
	  if(i == t->capacity - 1){
	       i = 0;
	  }
	  else{
	       i++;
	  }
	  t->collisions += 1;
	  entry = *(entries + i);
     }

     *(entries + i) = (Entry*) malloc(sizeof(Entry));


     if(*(entries + i) == NULL){
	  fprintf(stderr, "table::put() failed to allocate memory\n");
	  assert(NULL);
     }

     entry = *(entries + i);
  
     entry->key = key;
     entry->value = value;

     t->size += 1;

     return NULL;
}


/// Get the collection of values from the table.  This function allocates
/// space to store the values, which the caller is responsible for freeing.
/// @param t The table
/// @exception Assert fails if can't allocate space
/// @return A dynamic array of values
void** values(Table* t){
     void** val_list = calloc(t->size, sizeof(void*));

     if(val_list == NULL){
	  fprintf(stderr, "table::values() failed to allocate memory\n");
	  assert(NULL);
     }
  
     size_t i, j;
     Entry** entries = t->table;
     Entry* entry = *(entries);

     for(i = 0, j = 0; j < t->size;){
	  if(entry != NULL){
	       *(val_list + j) = entry->value;
	       j++;
	  }
	  i++;
	  if(i < t->capacity){
	       entry = *(entries + i);
	  }
     }
     return val_list;
}
