#include <vector>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <iostream>
#include <initializer_list>

#include "catch.hpp"

template <typename Entry>
class verificator {
public:
    verificator() = default;

    // nothing fatal, but may confuse
    verificator(const verificator& other) = delete;
    verificator& operator=(const verificator& other) = delete;

    verificator(verificator&& other) = delete;
    verificator& operator=(verificator&& other) = delete;

    ~verificator() {
        if (not good()) {
            INFO("Verificator failed, verificator log:");
            INFO("Events:");
            for (auto ev: entries)
                INFO(ev << ", tm=" << (mp.count(ev) ? mp.find(ev)->second : -1));
            INFO("");
            INFO("Order:");
            for (auto p: order)
                INFO(p.first << " " << p.second);
            
            REQUIRE(false);
        }
    }
    
    void expect(Entry en) {
        entries.push_back(en);
    }

    void depend(Entry a, Entry b) {
        order.emplace_back(a, b);
    }

    void depend_chain(std::initializer_list<Entry> lst) {
        if (lst.size() == 1)
            return;
        
        for (auto it = std::next(lst.begin()); it != lst.end(); ++it)
            depend(*std::prev(it), *it);
    }

    void log(Entry a) {
        REQUIRE(mp.count(a) == 0);
        mp[a] = time++;
    }

    bool good() {
        std::sort(entries.begin(), entries.end());
        if (std::unique(entries.begin(), entries.end()) != entries.end())
            return false;

        for (auto e: entries)
            if (mp.count(e) == 0)
                return false;

        for (auto p: mp)
            if (not std::binary_search(entries.begin(), entries.end(), p.first))
                return false;
        
        for (auto p: order)
            if (mp[p.first] >= mp[p.second])
                return false;
        return true;
    }
    
private:
    int time = 0;
    
    std::map<Entry, int> mp;
    std::vector<Entry> entries;
    std::vector<std::pair<Entry, Entry>> order;
};

