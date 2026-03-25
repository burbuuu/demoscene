#pragma once

#include <vector>

struct DemoEvent {
    int startTime;        // In milliseconds
    int effectIndex;      // Index of the effect in the Demo's effect list
    int transitionType;   // 0: Alpha, 1: Diagonal Cortinilla 1, 2: Diagonal Cortinilla 2, 3: Ellipse
    int transitionDuration; // In milliseconds
};

class Timeline {
public:
    void addEvent(int startTime, int effectIndex, int transitionType, int transitionDuration);
    const DemoEvent* getNextEvent(int currentTime) const;
    void markEventProcessed();

private:
    std::vector<DemoEvent> m_events;
    int m_currentEventIndex = 0;
};
