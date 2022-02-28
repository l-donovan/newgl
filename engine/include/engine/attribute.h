#pragma once

#include "global.h"
#include "common.h"
#include "event.h"

#include <optional>
#include <queue>
#include <string>

using std::string;

class Attribute {
    private:
        std::queue<EventType> subscription_queue;
    public:
        Attribute() {};

        void subscribe(EventType event_type) {
            this->subscription_queue.push(event_type);
        };

        virtual void receive_event(Event event) = 0;

        std::optional<EventType> pop_subscription_request() {
            if (this->subscription_queue.empty())
                return {};

            EventType request = this->subscription_queue.front();
            this->subscription_queue.pop();
            return request;
        };
};
