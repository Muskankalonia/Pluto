#pragma once 

#include <string>
#include <chrono>

enum class EventType {
    MARKET_DATA,
    ORDER,
    HEARTBEAT,
    UNKNOWN
};

struct Event {
    EventType type;
    std::chrono::high_resolution_clock::time_point timestamp;

    Event(EventType t = EventType::UNKNOWN) : type(t), timestamp(std::chrono::high_resolution_clock::now()) {}

    virtual ~Event() = default;
};