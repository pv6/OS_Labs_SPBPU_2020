#ifndef SET_H_INCLUDED__
#define SET_H_INCLUDED__


template <typename T>
class Set {
    public:
        virtual bool add(T& item) = 0;
        virtual bool remove(T& item) = 0;
        virtual bool contains(T& item) = 0;
        virtual bool empty() = 0;

        virtual ~Set(){};
};

#endif