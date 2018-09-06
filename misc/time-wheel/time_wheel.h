#ifndef TIME_WHEEL_H
#define TIME_WHEEL_H


class TimerEventInterface {
public:
    TimerEventInterface() {

    }

    // TimerEvents are automatically cancelled on destruction
    ~TimerEventInterface() {
        cancel();
    }

    // 
    inline void cancel();

    // return true if the event is 
    // currently scheduled for execution
    bool active() const {
        return slot_ != NULL;
    }

    // Return the absolute tick this event
    // is scheduled to be executed on
    Tick scheduled_at() const {
        return scheduled_at_;
    }


private:
    // forbidden copy constructor
    // forbidden assignment operator
    TimerEventInterface(const TimerEventInterface& other) = delete;
    TimerEventInterface& operator=(const TimerEventInterface& other) = delete;
    
    friend TimerWheelSlot;
    friend TimerWheel;

    // Implement in subclass. Execute the event callback
    virtual void execute() = 0;

    void set_schedual_at(Tick ts) { scheduled_at_ = ts;}

    // Move the event to another slot(It's safe for either the current 
    // or new slot to be NULL)
    inline relink(TimerWheelSlot* slot);

    Tick scheduled_at_;

    // The slot this event is currently in 
    TimerWheelSlot* slot = NULL;

    // The Events are linked together in the slot using 
    // an internal doubly-linked list;
    // 
    TimerEventInterface *next_ =NULL;
    TimerEventInterface *prev_ = NULL;
};

// An event that takes the callback to execute as a constructor parameter
template <typename CBType>
class TimerEvent : public TimerEventInterface {
public:
    explicit TimerEvent<CBType>(const CBType& callback)
        : callback_(callback) {}

private:
    TimerEvent<CBType>(const TimerEvent<CBType>& other) =delete;
    TimerEvent<CBType>& operator=(const TimerEvent<CBType>& other) =delete;

    CBType callback_;
};


// Purely an implementation detail
class TimerWheelSlot {
public:
    TimerWheelSlot() {

    }

private:
    // Return the first event queued in this slot
    const TimerEventInterface* events() const { return events_; }

    //Deque the first event from the slot and return it
    TimerEventInterface* pop_event() {
        auto event = events_;
        events_ = events_->next_;
        if (events_) {
            events_->prev_ = NULL;
        }
        event->next_ = NULL;
        event->slot_ = NULL;
        return event;
    }

    TimerWheelSlot(const TimerWheelSlot& other) = delete;
    TimerWheelSlot& operator=(const TimerWheelSlot& other) = delete;
    friend TimerEventInterface;
    friend TimerWheel;

    //Doubly linked list of events
    TimerEventInterface* events_ = NULL;
};


// A TimerWheel is the entity that TimerEvents can be 
// scheduled on for execution(with schedule() and schedule_in_range()),
// and will eventually be executed once the time advances far enough with 
// the advance() method

class TimerWheel {
public:
    TimerWheel(Tick now=0) {
        for(int i=0; i < NUM_LEVELS; ++i) {
            now_[i] = now >> (WIDTH_BITS * i);
        }
        ticks_pending_ = 0;
    }

    // 
    inline bool advance(Tick delta,
                        size_t max_execute=std::numeric_limits<size_t>::max(),
                        int level=0);

    // schedule the event to be executed delta ticks from the current time.
    inline void schedule(TimerEventInterface* event, Tick delta);

    //
    inline void schedule_in_range(TimerEventInterface* event, Tick start, Tick end);


    //
    Tick now() const {
        return now_[0];
    }

    //Return the number of ticks remaining until the next event
    // will get executed
    inline Tick ticks_to_next_event(Tick max = std::numeric_limits<Tick>::max(),
                                    int level = 0);
private:
    TimerWheel(const TimerWheel& other) = delete;
    TimerWheel& operator=(const TimerWheel& other) =delete;

    //This handles the actual work of executing event callbacks 
    // and recursing to the outer wheels
    inline bool process_current_slot(Tick now, size_t max_execute, int level);

    static const int WIDTH_BITS = 8;
    static const int NUM_LEVELS = (64 + WIDTH_BITS-1)/WIDTH_BITS;
    static const int MAX_LEVEL = NUM_LEVELS - 1;
    static const int NUM_SLOTS = 1 << WIDTH_BITS;

    // A bitmask for looking at just the bits in the timestamp relevant
    // to this wheel.
    static const int MASK = (NUM_SLOTS -1);

    //The current timestamp for this wheel. 
    // This will be right-shifted such that each slot
    // is separated by exactly one tick even on the outermost wheels
    Tick now_[NUM_LEVELS];

    // We've done a partial tick advance. This is how many ticks remain
    // unprocessed.
    Tick ticks_pending_;
    TimerWheelSlot slots_[NUM_LEVELS][NUM_SLOTS];
};


void TimerEventInterface::relink(TimerWheelSlot* new_slot)  {

    // unlink from old location
    if (slot_) {
        auto prev = prev_;
        auto next = next_;
        if (next) {
            next->prev_ = prev;
        }

        if (prev) {
            prev->next_ = next;
        } else {
            // head of this slot 
            slot_->events_ = next;
        }
    }

    // insert into new slot
    if (new_slot) {
        
    }

}

#endif