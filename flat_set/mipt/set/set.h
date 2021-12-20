#pragma once

#include "mipt/journal/journal.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include <optional>

namespace mipt {

enum LogEvent {
    INSERT_KEY = 0x00,
    ERASE_KEY = 0x01,
};

std::string to_string(LogEvent event) {
    switch (event) {
    case INSERT_KEY:
        return "INSERT";
    case ERASE_KEY:
        return "ERASE";
    default:
        assert(0);
    }
}

template<class T>
struct LogEntry {
    LogEvent event;
    T key;
    bool applied = false;
} __attribute__((packed));

template<class T>
class FlatSet {
public:
    using JournalType = Journal<FlatSet<T>, LogEntry<T>>;

public:
    FlatSet(std::optional<std::string_view> path = std::nullopt, OptimizeLevel optlevel = OptimizeLevel::NONE) 
    {
        if (path.has_value()) {
            journal_ = new JournalType(*path, this, optlevel);
            journal_->LoadState();
        }

        keys_.reserve(kInitialSize);
    }

    ~FlatSet()
    {
        if (journal_ != nullptr) {
            delete journal_;
        }
    }

    bool insert(const T& value, bool already_in_journal = false) {
        if (exists(value)) {
            return false;
        }
        keys_.push_back(value);
        std::sort(keys_.begin(), keys_.end());

        if (journal_ && !already_in_journal) {
            journal_->Write(LogEntry<T>{LogEvent::INSERT_KEY, value});
        }
        
        return true;
    }

    bool exists(const T& value) const {
        auto found = std::lower_bound(keys_.begin(), keys_.end(), value);
        if (found == keys_.end()) {
            return false;
        }

        return *found == value;
    }

    bool erase(const T& value, bool already_in_journal = false) {
        auto found = std::lower_bound(keys_.begin(), keys_.end(), value);
        if (found == keys_.end() || *found != value) {
            return false;
        }
        keys_.erase(found);

        if (journal_ && !already_in_journal) {
            journal_->Write(LogEntry<T>{LogEvent::ERASE_KEY, value});
        }
        
        return true;
    }
    
    size_t size() const {
        return keys_.size();
    }

    bool is_empty() const {
        return size() == 0;
    }

private:
    void Write(std::ostream& out) const {
        std::cerr << " lol i am here\n";
        size_t size = keys_.size();
        out.write((char*)&size, sizeof(size));
        out.write((char*)keys_.data(), size * sizeof(T));
    }

    void Read(std::istream& in) {
        size_t size;
        in.read((char*)&size, sizeof(size));
        keys_.resize(size);
        in.read((char*)keys_.data(), size * sizeof(T));
    }

    bool Apply(LogEntry<T>* entry) {
        INFOV() << "Read entry: action(" << to_string(entry->event) << ") value(" << entry->key << ")" << std::endl;

        bool status = 0;
        switch (entry->event) {
        case LogEvent::INSERT_KEY:
            status = insert(entry->key, true);
            break;
        case LogEvent::ERASE_KEY:
            status = erase(entry->key, true);
            break;
        }

        entry->applied = true;
        return status;
    }

    void ApplyBatch(std::vector<LogEntry<T>*>& entries) {
        auto for_insert = FlatSet<T>();
        auto for_erase = FlatSet<T>();

        for (auto* entry : entries) {
            if (entry->event == LogEvent::INSERT_KEY) {
                for_insert.insert(entry->key);
                for_erase.erase(entry->key);
            } else {
                for_insert.erase(entry->key);
                for_erase.insert(entry->key);
            }
        }

        for (const auto& key : for_erase.keys_) {
            erase(key, true);
        }
        for (const auto& key : for_insert.keys_) {
            insert(key, true);
        }
    }


private:
    friend class Journal<FlatSet<T>, LogEntry<T>>;
    static const size_t kInitialSize = 1024;

private:
    JournalType* journal_ = nullptr;   
    std::vector<T> keys_;
};

}