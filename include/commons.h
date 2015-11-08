#define SINGLETON(CLASS)\
public:\
    static CLASS *instance(){\
        static CLASS obj;\
        return &obj;\
    }\
    CLASS(const CLASS&) = delete;\
    CLASS &operator=(const CLASS&) = delete;

#define NON_COPYABLE(CLASS)\
public:\
    CLASS(const CLASS&) = delete;\
    CLASS &operator=(const CLASS&) = delete;