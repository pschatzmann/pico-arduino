#pragma once

#include "Vector.h"

namespace pico_arduino {

/**
 * @brief A simple key value map collection
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

template <class K, class V> 
class Map {
    protected:

        /**
         * @brief Key/Value
         * 
         * @tparam KM 
         * @tparam VM 
         */
        template <class KM, class VM> 
        struct MapEntry {
            VM value;
            KM key;
        };
        V* empty_ptr;
        Vector<MapEntry<K,V>> data;

        V &find_entry(K key){
            for ( auto it = data.begin(); it != data.end(); ++it){
                if ((*it).key==key){
                    return (*it).value;
                }    
            }
            return  *empty_ptr;                                
        } 


    public:
        /**
         * @brief Construct a new Map object
         * 
         * @param empty Default value which is provided if the key does not exist
         */
        Map(const V &empty){
            this->empty_ptr = (V*) &empty;
        }

        /// Gets an element by key
        V &get(K key){
            return find_entry(key);                                
        } 

        /// Adds an element with a key
        void put(K key,V value){
            V value_found = find_entry(key);
            // check if enty exists
            if (&value_found == empty_ptr ){
                // add new entry
                MapEntry<K,V> new_entry = {value, key};
                data.push_back(new_entry);
            }
        }

};

}