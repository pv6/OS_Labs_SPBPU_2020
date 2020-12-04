#ifndef DATEANDTEMP_H
#define DATEANDTEMP_H


class DateAndTemp {
public:
    int getTemp() const;
    unsigned int getDay() const;
    unsigned int getMonth() const;
    unsigned int getYear() const;
    void setTemp(int temperature);

    DateAndTemp(const unsigned int day = 0, const unsigned int month = 0, const unsigned int year = 0) :
    day{day}, month{month}, year{year}, temp{0} { };
    //DateAndTemp() : day{0}, month{0}, year{0}, temp{0} { };
    static const int minTemp = -90;
    static const int maxTemp = 60;

private:
    unsigned int day;
    unsigned int month;
    unsigned int year;
    int temp;
};



#endif //DATEANDTEMP_H