#ifndef DATEHOLDER_H
#define DATEHOLDER_H

#include <string>

class DateHolder
{
    public:
        DateHolder(const size_t day, const size_t month, const size_t year);
        DateHolder(const std::string& repr);  // parse serialized string: "dd.mm.yyyy"
        virtual ~DateHolder();

        std::string serialize(); // get repr in format "dd.mm.yyyy"

        // getters
        size_t getDay() const;
        size_t getMonth() const;
        size_t getYear() const;
    protected:

    private:
        // fields
        size_t day;
        size_t month;
        size_t year;

        // functions
        void validateDMY(const size_t day, const size_t month, const size_t year);
};

#endif // DATEHOLDER_H
