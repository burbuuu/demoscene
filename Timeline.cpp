#include "Timeline.h"
#include <algorithm>

void Timeline::addEvent(int startTime, int effectIndex, int transitionType, int transitionDuration) {
    m_events.push_back({startTime, effectIndex, transitionType, transitionDuration});
    // Ensure events are sorted by start time
    std::sort(m_events.begin(), m_events.end(), [](const DemoEvent& a, const DemoEvent& b) {
        return a.startTime < b.startTime;
    });
}

const DemoEvent* Timeline::getNextEvent(int currentTime) const {
    if (m_currentEventIndex < m_events.size()) {
        const DemoEvent& event = m_events[m_currentEventIndex];
        if (currentTime >= event.startTime) {
            return &event;
        }
    }
    return nullptr;
}

void Timeline::markEventProcessed() {
    m_currentEventIndex++;
}
