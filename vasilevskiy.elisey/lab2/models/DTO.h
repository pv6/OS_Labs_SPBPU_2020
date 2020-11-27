#ifndef LAB2_DTO_H
#define LAB2_DTO_H


class DTO {
public:
    DTO(const unsigned int day = 0, const unsigned int month = 0, const unsigned int year = 0) :
            day{day}, month{month}, year{year}, temperature{0} {};

    int getTemp() const;

    void setTemp(int temperature);

    unsigned int getDay() const;

    unsigned int getMonth() const;

    unsigned int getYear() const;

private:
    unsigned int day;
    unsigned int month;
    unsigned int year;
    int temperature;
};


#endif //LAB2_DTO_H
