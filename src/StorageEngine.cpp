#include "StorageEngine.hpp"
#include <chrono>

void StorageEngine::set(const std::string& key, std::string value, double duration) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    
    m_data[key].push_back(std::make_unique<Record>(std::move(value), now, duration)); //testing unique+move for max effi
}

std::optional<std::string> StorageEngine::get(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto it = m_data.find(key);
    if (it != m_data.end() && !it->second.empty()) {
        return it->second.back()->value;
    }
    
    return std::nullopt; 
}
double StorageEngine::getAverage(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto it = m_data.find(key);
    if (it != m_data.end() && !it->second.empty()) {
        double sum = 0.0;
        for(const auto& record : it->second) {
            sum += record->duration;
        }
        return sum / it->second.size();   
    }
    return 0.0;
}

bool StorageEngine::remove(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    return m_data.erase(key) > 0;
}

size_t StorageEngine::count() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_data.size();
}
